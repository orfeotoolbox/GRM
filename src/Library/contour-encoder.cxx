/*=========================================================================

  Program:  Shape Encoder librarie of OBIA (Object Based Image Analysis)
  Language:  C++
  author:    Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "contour-encoder.h"

namespace shpenc
{
	typename ShapeEncoder::Contour
	ShapeEncoder::EncodeContour(long unsigned int curr_id, 
								Contour&     curr_contour,
								long unsigned int adj_id,
								Contour&     adj_contour,
								BoundingBox& merged_bbox)
	{
		// To determine the future contour pixels
		std::vector<short> matrix_bbox(merged_bbox[2]*merged_bbox[3], 0);

		// Generate contour pixels of the merged segments
		auto curr_v = GeneratePixels(curr_id, curr_contour);
		auto adj_v = GeneratePixels(adj_id, adj_contour);
		curr_v.insert(curr_v.end(), adj_v.begin(), adj_v.end());

		// A case in the matrix_bbox filled by a pixel is marked true
		for(auto& pix : curr_v)
			matrix_bbox[ToBBoxId(pix, merged_bbox)] = 1;

		// Remove internal pixels
		RemoveInternalPixels(matrix_bbox, merged_bbox[3], merged_bbox[2]);

		// Encode the new contour
		return EncodeNewContour(curr_id, merged_bbox, matrix_bbox);
	}

	std::vector<long unsigned int> ShapeEncoder::GeneratePixels(long unsigned int id, Contour& contour)
	{
		std::vector<long unsigned int> pixels;
		pixels.push_back(id);
		
		short curr_move, prev_move;
		
		if(contour.size() > 4)
		{
			for(auto m = contour.begin()+1; m != contour.end(); m++)
			{
				curr_move = (*m)[0] + 2*(*m)[1];
				prev_move = (*(m-1))[0] + 2*(*(m-1))[1];

				if(curr_move == 0)
				{
					if(prev_move == 0)
					{
						id-=m_Cols;
						pixels.push_back(id);
					}
					else if(prev_move == 1) // 1
					{
						id++;
						pixels.push_back(id);
						id-=m_Cols;
						pixels.push_back(id);
					}
				}
				else if(curr_move == 1)
				{
					if(prev_move == 1)
					{
						id++;
						pixels.push_back(id);
					}
					else if(prev_move == 2)
					{
						id+=m_Cols;
						pixels.push_back(id);
						id++;
						pixels.push_back(id);
					}
				}
				else if(curr_move == 2)
				{
					if(prev_move == 3)
					{
						id--;
						pixels.push_back(id);
						id+=m_Cols;
						pixels.push_back(id);
					}
					else if(prev_move == 2)
					{
						id+=m_Cols;
						pixels.push_back(id);
					}
				}
				else if(curr_move == 3)
				{
					if(prev_move==0)
					{
						id-=m_Cols;
						pixels.push_back(id);
						id--;
						pixels.push_back(id);
					}
					else if(prev_move == 3)
					{
						id--;
						pixels.push_back(id);
					}
				}
			}
			// Remove duplicated pixels
			std::sort(pixels.begin(), pixels.end());
			auto it = std::unique(pixels.begin(), pixels.end());
  			pixels.resize(std::distance(pixels.begin(), it));
		}
		return pixels;
	}

	unsigned int ShapeEncoder::ToBBoxId(long unsigned int id, BoundingBox& bounding_box)
	{
		unsigned int img_row = id / m_Cols;
		unsigned int img_col = id % m_Cols;
		unsigned int bbox_row = img_row - bounding_box[1];
		unsigned int bbox_col = img_col - bounding_box[0];
		return bbox_row * bounding_box[2] + bbox_col;
	}

	long unsigned int ShapeEncoder::ToImgId(unsigned int id, BoundingBox& bounding_box)
	{
		unsigned int bbox_row = id / bounding_box[2];
		unsigned int bbox_col = id % bounding_box[2];
		unsigned int img_row = bbox_row + bounding_box[1];
		unsigned int img_col = bbox_col + bounding_box[0];

		return img_row * m_Cols + img_col;
	}

	void ShapeEncoder::RemoveInternalPixels(std::vector<short>& matrix_bbox, 
							  				  unsigned int bbox_rows, 
							                  unsigned int bbox_cols)
	{
		int mat_idx;
		bool is_internal;
		for(int r = 1; r<bbox_rows-1; r++)
		{
			for(int c = 1; c<bbox_cols-1; c++)
			{
				mat_idx = r*bbox_cols+c;
				is_internal = true;
				if(matrix_bbox[mat_idx] == 1)
				{
					auto neighbors = mask::SquareNeighborhood(mat_idx, bbox_cols, bbox_rows);
					for(short j=0; j<8; j++)
					{
						if(matrix_bbox[neighbors[j]] == 0)
						{
							is_internal = false;
							break;
						}
					}
					if(is_internal)
						matrix_bbox[mat_idx] = 2;
				}
			}	
		}
		for(int i = 0; i<bbox_rows*bbox_cols; i++)
			if(matrix_bbox[i] == 2) matrix_bbox[i] = 0;
	}

	typename ShapeEncoder::Contour 
	ShapeEncoder::EncodeNewContour(long unsigned int curr_id,
									BoundingBox& merged_bbox,
									std::vector<short>& matrix_bbox)
	{
		short direction, pos;              // position on one of the 4 corners of the pixel
		std::array<short, 4> sight;        // direction of the field of view

		unsigned int start_mat_id = ToBBoxId(curr_id, merged_bbox);
		unsigned int curr_mat_id = start_mat_id;

		unsigned int matrix_bbox_rows = merged_bbox[3];
		unsigned int matrix_bbox_cols = merged_bbox[2];
		
		Contour curr_contour; // Reset the previous contour

		// Build the first move
		auto neighbors = mask::CrossNeighborhood(curr_mat_id, matrix_bbox_cols, matrix_bbox_rows);

		// Only 2 cases: neighbor at the right or at the bottom
		if(matrix_bbox[neighbors[1]] == 1) // right
		{
			pos = 1;       // top left corner of the pixel
			curr_mat_id++; // go to the pixel at the right
			direction = 1; // direction is along the right
			curr_contour.push_back(1); // Add the move to the right
		}
		else if(matrix_bbox[neighbors[2]] == 1) // bottom
		{
			pos = 2; // top right corner of the pixel
			curr_mat_id += matrix_bbox_cols; // go to the pixel at the bottom
			direction = 2; // direction is along the bottom						
			curr_contour.push_back(1); // add move to the right
			curr_contour.push_back(2); // add move to the bottom	
		}

		// Keep going this same reasonning until we reach the start pixel (start_mat_id)
		while(curr_mat_id != start_mat_id)
		{
			UpdateSight(sight, direction);
			auto neighbors = mask::CrossNeighborhood(curr_mat_id, matrix_bbox_cols, matrix_bbox_rows);

			// All the cases are possible: top, right, bottom, left
			for(short d=0; d<4; d++)
			{
				if(neighbors[sight[d]] > -1)
				{
					if(matrix_bbox[neighbors[sight[d]]] > 0)
					{
						if(sight[d] == 0)	
						{
							EncodeContourForTopPixel(curr_mat_id, direction, pos, curr_contour, matrix_bbox_cols);
							break;
						}
						else if(sight[d] == 1)
						{
							EncodeContourForRightPixel(curr_mat_id, direction, pos, curr_contour, matrix_bbox_cols);
							break;
						}
						else if(sight[d] == 2)
						{
							EncodeContourForBottomPixel(curr_mat_id, direction, pos, curr_contour, matrix_bbox_cols);
							break;
						}
						else if(sight[d] == 3)
						{
							EncodeContourForLeftPixel(curr_mat_id, direction, pos, curr_contour, matrix_bbox_cols);
							break;
						}
					}
				}
			}

			// It is possible to reach the start pixel whithout finishing to encode the whole contour.
			// 00111
			// 11100
			// In this case the direction value is always 3, the position value is always 3
			// We have to check if there is a neighbor at left of the current direction
			if(curr_mat_id == start_mat_id)
			{
				if(pos == 3 && direction == 3)
				{
					UpdateSight(sight, direction);
					auto neighbors = mask::CrossNeighborhood(curr_mat_id, matrix_bbox_cols, matrix_bbox_rows);

					if(neighbors[sight[0]] > -1)
					{
						if(matrix_bbox[neighbors[sight[0]]] == 1)
						{
							pos = 3;
							direction = 2;
							curr_contour.push_back(2);
							curr_mat_id += matrix_bbox_cols;
						}
					}
				}
			}		
		}

		// We reach the start pixel but maybe we did not encode the whole contour
		if(pos == 3)
		{
			curr_contour.push_back(3);
			curr_contour.push_back(0);
		}
		else if(pos == 4)
			curr_contour.push_back(0);

		return curr_contour;
	}

	void ShapeEncoder::UpdateSight(std::array<short, 4>& sight, short direction)
	{
		sight[0] = (direction + 3) % 4; // Look at the left
		sight[1] = direction; // Look in front of
		sight[2] = (direction + 1) % 4; // Look at the right
		sight[3] = (direction + 2) % 4; // Look behind 
	}

	void ShapeEncoder::EncodeContourForTopPixel(unsigned int& curr_mat_id,
												  short& direction,
												  short& pos,
									              Contour& curr_contour,
												  const unsigned int matrix_bbox_cols)
	{
		if(pos == 1)
		{
			if(direction==0)
				pos = 4;
			else if(direction == 1)
			{
				pos = 1;
				curr_contour.push_back(0);
			}
		}
		else if(pos == 2)
		{
			pos = 4;
			curr_contour.push_back(2);
			curr_contour.push_back(3);
			curr_contour.push_back(0);
		}
		else if(pos == 3)
		{
			pos = 4;
			curr_contour.push_back(3);
			curr_contour.push_back(0);
		}
		else if(pos == 4)
			curr_contour.push_back(0);


		curr_mat_id -= matrix_bbox_cols;
		direction = 0;
	}
	void ShapeEncoder::EncodeContourForRightPixel(unsigned int& curr_mat_id,
												    short& direction,
												    short& pos,
									                Contour& curr_contour,
												    const unsigned int matrix_bbox_cols)
	{
		if(pos == 1)
			curr_contour.push_back(1);
		else if(pos == 2)
		{
			if(direction == 1)
				pos = 1;
			else if(direction == 2)
				curr_contour.push_back(1);
		}
		else if(pos == 3)
		{
			pos = 1;
			curr_contour.push_back(3);
			curr_contour.push_back(0);
			curr_contour.push_back(1);
		}
		else if(pos == 4)
		{
			pos = 1;
			curr_contour.push_back(0);
			curr_contour.push_back(1);
		}

		curr_mat_id++;
		direction = 1;	
	}
	void ShapeEncoder::EncodeContourForBottomPixel(unsigned int& curr_mat_id,
												     short& direction,
												     short& pos,
									                 Contour& curr_contour,
												     const unsigned int matrix_bbox_cols)
	{
		if(pos == 1)
		{
			pos = 2;
			curr_contour.push_back(1);
			curr_contour.push_back(2);
		}
		else if(pos == 2)
			curr_contour.push_back(2);
		else if(pos == 3)
		{
			if(direction == 2)
				pos = 2;
			else if(direction == 3)
				curr_contour.push_back(2);
		}
		else if(pos == 4)
		{
			pos = 2;
			curr_contour.push_back(0);
			curr_contour.push_back(1);
			curr_contour.push_back(2);
		}

		curr_mat_id += matrix_bbox_cols;
		direction = 2;			
	}
	void ShapeEncoder::EncodeContourForLeftPixel(unsigned int& curr_mat_id,
												   short& direction,
												   short& pos,
									               Contour& curr_contour,
												   const unsigned int matrix_bbox_cols)
	{
		if(pos == 1)
		{
			pos = 3;
			curr_contour.push_back(1);
			curr_contour.push_back(2);
			curr_contour.push_back(3);
		}
		else if(pos == 2)
		{
			pos = 3;
			curr_contour.push_back(2);
			curr_contour.push_back(3);
		}
		else if(pos == 3)
			curr_contour.push_back(3);
		else if(pos == 4)
		{
			if(direction == 3)
				pos = 3;
			else if(direction == 0)
			{
				pos = 4;
				curr_contour.push_back(3);
			}
		}

		curr_mat_id --;
		direction = 3;
	}


	bool ShapeEncoder::GetCollisionAtNorth(unsigned int idx,
											const std::vector<short>& bbox_array,
											unsigned int bbox_cols,
											unsigned int bbox_rows)
	{
		assert(idx >= 0 && idx < bbox_cols * bbox_rows);
		long int tmp_idx = idx - bbox_cols;

		if(bbox_array[tmp_idx] == 1)
			return true;

		while(tmp_idx > 0)
		{
			if(bbox_array[tmp_idx] == 1)
				return true;
			tmp_idx -= bbox_cols;
		}
		return false;
	}

	bool ShapeEncoder::GetCollisionAtNorthEast(unsigned int idx,
											const std::vector<short>& bbox_array,
											unsigned int bbox_cols,
											unsigned int bbox_rows)
	{
		assert(idx >= 0 && idx < bbox_cols * bbox_rows);
		long int tmp_idx = idx - bbox_cols + 1;

		if(bbox_array[tmp_idx] == 1)
			return true;

		while(tmp_idx > 0 && (tmp_idx % bbox_cols) != 0)
		{
			if(bbox_array[tmp_idx] == 1)
				return true;
			tmp_idx = tmp_idx - bbox_cols + 1;
		}
		return false;
	}

	bool ShapeEncoder::GetCollisionAtEast(unsigned int idx,
										const std::vector<short>& bbox_array,
										unsigned int bbox_cols,
										unsigned int bbox_rows)
	{
		assert(idx >= 0 && idx < bbox_cols * bbox_rows);
		long int tmp_idx = idx + 1;


		if(bbox_array[tmp_idx] == 1)
			return true;

		while((tmp_idx % bbox_cols) != 0)
		{
			if(bbox_array[tmp_idx] == 1)
				return true;
			++tmp_idx;
		}
		return false;
	}

	bool ShapeEncoder::GetCollisionAtSouthEast(unsigned int idx,
										const std::vector<short>& bbox_array,
										unsigned int bbox_cols,
										unsigned int bbox_rows)
	{
		assert(idx >= 0 && idx < bbox_cols * bbox_rows);
		long int tmp_idx = idx + 1 + bbox_cols;

		if(bbox_array[tmp_idx] == 1)
			return true;

		while(tmp_idx < (bbox_cols * bbox_rows) &&
			(tmp_idx % bbox_cols) != 0)
		{
			if(bbox_array[tmp_idx] == 1)
				return true;
			tmp_idx = tmp_idx + 1 + bbox_cols;
		}
		return false;
	}

	bool ShapeEncoder::GetCollisionAtSouth(unsigned int idx,
										const std::vector<short>& bbox_array,
										unsigned int bbox_cols,
										unsigned int bbox_rows)
	{
		assert(idx >= 0 && idx < bbox_cols * bbox_rows);
		long int tmp_idx = idx + bbox_cols;

		if(bbox_array[tmp_idx] == 1)
			return true;

		while(tmp_idx < (bbox_cols * bbox_rows))
		{
			if(bbox_array[tmp_idx] == 1)
				return true;
			tmp_idx += bbox_cols;
		}
		return false;
	}

	bool ShapeEncoder::GetCollisionAtSouthWest(unsigned int idx,
											const std::vector<short>& bbox_array,
											unsigned int bbox_cols,
											unsigned int bbox_rows)
	{
		assert(idx >= 0 && idx < bbox_cols * bbox_rows);
		long int tmp_idx = idx + bbox_cols - 1;

		if(bbox_array[tmp_idx] == 1)
			return true;

		while(tmp_idx < (bbox_cols * bbox_rows) &&
				(tmp_idx % bbox_cols) != bbox_cols - 1)
		{
			if(bbox_array[tmp_idx] == 1)
				return true;
			tmp_idx = tmp_idx + bbox_cols - 1;
		}
		return false;
	}

	bool ShapeEncoder::GetCollisionAtWest(unsigned int idx,
											const std::vector<short>& bbox_array,
											unsigned int bbox_cols,
											unsigned int bbox_rows)
	{
		assert(idx >= 0 && idx < bbox_cols * bbox_rows);
		long int tmp_idx = idx - 1;

		if(bbox_array[tmp_idx] == 1)
			return true;

		while((tmp_idx % bbox_cols) != bbox_cols - 1)
		{
			if(bbox_array[tmp_idx] == 1)
				return true;
			tmp_idx -= 1;
		}
		return false;
	}

	bool ShapeEncoder::GetCollisionAtNorthWest(unsigned int idx,
											const std::vector<short>& bbox_array,
											unsigned int bbox_cols,
											unsigned int bbox_rows)
	{
		assert(idx >= 0 && idx < bbox_cols * bbox_rows);
		long int tmp_idx = idx - bbox_cols - 1;

		if(bbox_array[tmp_idx] == 1)
			return true;

		while(tmp_idx > 0 && (tmp_idx % bbox_cols) != bbox_cols - 1)
		{
			if(bbox_array[tmp_idx] == 1)
				return true;
			tmp_idx = tmp_idx - bbox_cols - 1;
		}
		return false;
	}

	bool ShapeEncoder::IsInternalPixel(unsigned int curr_idx, std::vector<short>& bbox_array, 
										unsigned int bbox_size_x, unsigned int bbox_size_y)
	{
		if(!GetCollisionAtNorth(curr_idx, bbox_array, bbox_size_x, bbox_size_y))
			return false;
		if(!GetCollisionAtNorthEast(curr_idx, bbox_array, bbox_size_x, bbox_size_y))
			return false;
		if(!GetCollisionAtEast(curr_idx, bbox_array, bbox_size_x, bbox_size_y))
			return false;
		if(!GetCollisionAtSouthEast(curr_idx, bbox_array, bbox_size_x, bbox_size_y))
			return false;
		if(!GetCollisionAtSouth(curr_idx, bbox_array, bbox_size_x, bbox_size_y))
			return false;
		if(!GetCollisionAtSouthWest(curr_idx, bbox_array, bbox_size_x, bbox_size_y))
			return false;
		if(!GetCollisionAtWest(curr_idx, bbox_array, bbox_size_x, bbox_size_y))
			return false;
		if(!GetCollisionAtNorthWest(curr_idx, bbox_array, bbox_size_x, bbox_size_y))
			return false;

		return true;
	}

	std::vector<long unsigned int> ShapeEncoder::GenerateAllPixels(long unsigned int id, 
																	Contour& contour, 
																	BoundingBox& bounding_box)
	{
		// Generate the border pixels id in the coordinnate system of the image
		auto border_pixels = GeneratePixels(id,contour);
		// Build the matrix which corresponds to the bounding box
		const unsigned int bbox_array_size = bounding_box[2]*bounding_box[3];
		const unsigned int bbox_size_x = bounding_box[2];
		const unsigned int bbox_size_y = bounding_box[3];
		std::vector<short> bbox_array;
		bbox_array.assign(bbox_array_size, 0);

		// Fill the bbox_array with the border pixels
		for(auto& bp : border_pixels)
			bbox_array[ToBBoxId(bp, bounding_box)] = 1;

		// For each case of the bbox_array we determine if it is inside or outside of the region
		std::vector<long unsigned int> pixels;
		pixels.insert(pixels.begin(), border_pixels.begin(), border_pixels.end());
		bool internal = true;
		unsigned int tmp_idx, curr_idx;
		for(unsigned int r = 1; r < bbox_size_y - 1; r++)
		{
			for(unsigned int c = 1; c < bbox_size_x - 1; c++)
			{
				curr_idx = r*bbox_size_x + c;
				if(bbox_array[curr_idx] != 1)
				{
					auto internal = IsInternalPixel(curr_idx, bbox_array, bbox_size_x, bbox_size_y);
					bbox_array[curr_idx] = 1;
					if(internal)
					{
						pixels.push_back(ToImgId(curr_idx, bounding_box));
					}
				}
			}
		}

		return pixels;
	}

}










