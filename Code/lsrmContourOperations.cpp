#include "lsrmContourOperations.h"

namespace lsrm
{
	BoundingBox
	ContourOperations::MergeBoundingBoxes(const BoundingBox& bb1,
										  const BoundingBox& bb2)
	{
		long unsigned int min_ux, min_uy, max_xw, max_yh;
		BoundingBox bb;

		min_ux = std::min(bb1.m_UX, bb2.m_UX);
		min_uy = std::min(bb1.m_UY, bb2.m_UY);
		max_xw = std::max(bb1.m_UX + bb1.m_W, bb2.m_UX + bb2.m_W);
		max_yh = std::max(bb1.m_UY + bb1.m_H, bb2.m_UY + bb2.m_H);

		bb.m_UX = min_ux;
		bb.m_UY = min_uy;
		bb.m_W = max_xw - min_ux;
		bb.m_H = max_yh - min_uy;

		return bb;
	}

	long unsigned int
	ContourOperations::ToBoundingBoxCoordinates(const long unsigned int id,
												const BoundingBox& bb,
												const unsigned int width)
	{
		long unsigned int img_x = id % width;
		long unsigned int img_y = id / width;
		long unsigned int bb_x = img_x - bb.m_UX;
		long unsigned int bb_y = img_y - bb.m_UY;
		return (bb_y * bb.m_W + bb_x);
	}

	typename ContourOperations::PixelList
	ContourOperations::GenerateBorderPixels(long unsigned int id,
											const Contour& contour,
											const unsigned int width)
	{
		PixelList pixels;
		pixels.push_back(id);

		short curr_move, prev_move;
		
		if(contour.size() > 4)
		{
			for(ContourConstIterator m = contour.begin()+1; m != contour.end(); m++)
			{
				curr_move = (*m)[0] + 2*(*m)[1];
				prev_move = (*(m-1))[0] + 2*(*(m-1))[1];

				if(curr_move == 0)
				{
					if(prev_move == 0)
					{
						id-=width;
						pixels.push_back(id);
					}
					else if(prev_move == 1) // 1
					{
						id++;
						pixels.push_back(id);
						id-=width;
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
						id+=width;
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
						id+=width;
						pixels.push_back(id);
					}
					else if(prev_move == 2)
					{
						id+=width;
						pixels.push_back(id);
					}
				}
				else if(curr_move == 3)
				{
					if(prev_move==0)
					{
						id-=width;
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
			PixelIterator it = std::unique(pixels.begin(), pixels.end());
  			pixels.resize(std::distance(pixels.begin(), it));
		}
		return pixels;
	}

	bool
	ContourOperations::GetCollisionAtNorth(const long unsigned int idx,
										   const short * grid,
										   const unsigned int grid_width)
	{
		long int tmp_idx = idx - grid_width;

		if(grid[tmp_idx] > 0)
			return true;

		while(tmp_idx > 0)
		{
			if(grid[tmp_idx] > 0)
				return true;
			tmp_idx -= grid_width;
		}
		return false;
	}

	bool
	ContourOperations::GetCollisionAtNorthEast(const long unsigned int idx,
											   const short * grid,
											   const unsigned int grid_width)
	{
		long int tmp_idx = idx - grid_width + 1;

		if(grid[tmp_idx] > 0)
			return true;

		while(tmp_idx > 0 && (tmp_idx % grid_width) != 0)
		{
			if(grid[tmp_idx] > 0)
				return true;
			tmp_idx = tmp_idx - grid_width + 1;
		}
		return false;
	}

	bool
	ContourOperations::GetCollisionAtEast(const long unsigned int idx,
										  const short * grid,
										  const unsigned int grid_width)
	{
		long int tmp_idx = idx + 1;


		if(grid[tmp_idx] > 0)
			return true;

		while((tmp_idx % grid_width) != 0)
		{
			if(grid[tmp_idx] > 0)
				return true;
			++tmp_idx;
		}
		return false;
	}

	bool
	ContourOperations::GetCollisionAtSouthEast(const long unsigned int idx,
											   const short * grid,
											   const unsigned int grid_width,
											   const unsigned int grid_height)
	{
		long int tmp_idx = idx + 1 + grid_width;

		if(grid[tmp_idx] > 0)
			return true;

		while(tmp_idx < (grid_width * grid_height) &&
			(tmp_idx % grid_width) != 0)
		{
			if(grid[tmp_idx] > 0)
				return true;
			tmp_idx = tmp_idx + 1 + grid_width;
		}
		return false;
	}

	bool
	ContourOperations::GetCollisionAtSouth(const long unsigned int idx,
										   const short * grid,
										   const unsigned int grid_width,
										   const unsigned int grid_height)
	{
		long int tmp_idx = idx + grid_width;

		if(grid[tmp_idx] > 0)
			return true;

		while(tmp_idx < (grid_width * grid_height))
		{
			if(grid[tmp_idx] > 0)
				return true;
			tmp_idx += grid_width;
		}
		return false;
	}

	bool
	ContourOperations::GetCollisionAtSouthWest(const long unsigned int idx,
											   const short * grid,
											   const unsigned int grid_width,
											   const unsigned int grid_height)
	{
		long int tmp_idx = idx + grid_width - 1;

		if(grid[tmp_idx] > 0)
			return true;

		while(tmp_idx < (grid_width * grid_height) &&
			  (tmp_idx % grid_width) != grid_width - 1)
		{
			if(grid[tmp_idx] > 0)
				return true;
			tmp_idx = tmp_idx + grid_width - 1;
		}
		return false;
	}

	bool
	ContourOperations::GetCollisionAtWest(const long unsigned int idx,
										  const short * grid,
										  const unsigned int grid_width)
	{
		long int tmp_idx = idx - 1;

		if(grid[tmp_idx] > 0)
			return true;

		while((tmp_idx % grid_width) != grid_width - 1)
		{
			if(grid[tmp_idx] > 0)
				return true;
			tmp_idx -= 1;
		}
		return false;
	}

	bool
	ContourOperations::GetCollisionAtNorthWest(const long unsigned int idx,
											   const short * grid,
											   const unsigned int grid_width)
	{
		long int tmp_idx = idx - grid_width - 1;

		if(grid[tmp_idx] > 0)
			return true;

		while(tmp_idx > 0 && (tmp_idx % grid_width) != grid_width - 1)
		{
			if(grid[tmp_idx] > 0)
				return true;
			tmp_idx = tmp_idx - grid_width - 1;
		}
		return false;
	}

	bool
	ContourOperations::IsInternalPixel(const long unsigned int idx,
									   const short * grid,
									   const unsigned int grid_width,
									   const unsigned int grid_height)
	{
		if(!GetCollisionAtNorth(idx, grid, grid_width))
			return false;
		if(!GetCollisionAtNorthEast(idx, grid, grid_width))
			return false;
		if(!GetCollisionAtEast(idx, grid, grid_width))
			return false;
		if(!GetCollisionAtSouthEast(idx, grid, grid_width, grid_height))
			return false;
		if(!GetCollisionAtSouth(idx, grid, grid_width, grid_height))
			return false;
		if(!GetCollisionAtSouthWest(idx, grid, grid_width, grid_height))
			return false;
		if(!GetCollisionAtWest(idx, grid, grid_width))
			return false;
		if(!GetCollisionAtNorthWest(idx, grid, grid_width))
			return false;

		return true;
	}

	void
	ContourOperations::UpdateSight(short * sight, const short direction)
	{
		sight[0] = (direction + 3) % 4; // look at the left
		sight[1] = direction; // look in front
		sight[2] = (direction + 1) % 4; // look at the right
		sight[3] = (direction + 2) % 4; // look behind
	}

	void
	ContourOperations::EncodeContourForTopPixel(long unsigned int& curr_mat_id,
												short& direction,
												short& pos,
												Contour& curr_contour,
												const unsigned int grid_width)
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


		curr_mat_id -= grid_width;
		direction = 0;
	}

	void
	ContourOperations::EncodeContourForRightPixel(long unsigned int& curr_mat_id,
												  short& direction,
												  short& pos,
												  Contour& curr_contour)
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

	void
	ContourOperations::EncodeContourForBottomPixel(long unsigned int& curr_mat_id,
												   short& direction,
												   short& pos,
												   Contour& curr_contour,
												   const unsigned int grid_width)
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

		curr_mat_id += grid_width;
		direction = 2;	
	}

	void
	ContourOperations::EncodeContourForLeftPixel(long unsigned int& curr_mat_id,
												 short& direction,
												 short& pos,
												 Contour& curr_contour)
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

	Contour
	ContourOperations::CreateContourFromBorderPixels(const long unsigned int id,
													 const BoundingBox& bbox,
													 const short * grid,
													 const unsigned int width)
	{
		/* Location on one of the 4 corners of a pixel */
		short direction, pos;
		/* Field of view */
		short sight[4];

		long unsigned int start_mat_id = ToBoundingBoxCoordinates(id, bbox, width);
		long unsigned int curr_mat_id = start_mat_id;

		/* New contour */
		Contour curr_contour;

		// Build the first move
		long int neighbors[4];
		FOURNeighborhood(neighbors, curr_mat_id, bbox.m_W, bbox.m_H);

		// Only 2 cases: neighbor at the right or at the bottom
		if(grid[neighbors[1]] > 0) // right
		{
			pos = 1;       // top left corner of the pixel
			curr_mat_id++; // go to the pixel at the right
			direction = 1; // direction is along the right
			curr_contour.push_back(1); // Add the move to the right
		}
		else if(grid[neighbors[2]] > 0) // bottom
		{
			pos = 2; // top right corner of the pixel
			curr_mat_id += bbox.m_W; // go to the pixel at the bottom
			direction = 2; // direction is along the bottom						
			curr_contour.push_back(1); // add move to the right
			curr_contour.push_back(2); // add move to the bottom	
		}

		// Keep going this same reasonning until we reach the start pixel (start_mat_id)
		while(curr_mat_id != start_mat_id)
		{
			UpdateSight(sight, direction);
			FOURNeighborhood(neighbors, curr_mat_id, bbox.m_W, bbox.m_H);

			// All the cases are possible: top, right, bottom, left
			for(short d=0; d<4; d++)
			{
				if(neighbors[sight[d]] > -1)
				{
					if(grid[neighbors[sight[d]]] > 0)
					{
						if(sight[d] == 0)	
						{
							EncodeContourForTopPixel(curr_mat_id, direction, pos, curr_contour, bbox.m_W);
							break;
						}
						else if(sight[d] == 1)
						{
							EncodeContourForRightPixel(curr_mat_id, direction, pos, curr_contour);
							break;
						}
						else if(sight[d] == 2)
						{
							EncodeContourForBottomPixel(curr_mat_id, direction, pos, curr_contour, bbox.m_W);
							break;
						}
						else if(sight[d] == 3)
						{
							EncodeContourForLeftPixel(curr_mat_id, direction, pos, curr_contour);
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
					FOURNeighborhood(neighbors, curr_mat_id, bbox.m_W, bbox.m_H);

					if(neighbors[sight[0]] > -1)
					{
						if(grid[neighbors[sight[0]]] == 1)
						{
							pos = 3;
							direction = 2;
							curr_contour.push_back(2);
							curr_mat_id += bbox.m_W;
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
	
	Contour
	ContourOperations::MergeContours(const long unsigned int start_id1,
									 const long unsigned int start_id2,
									 const Contour& c1,
									 const Contour& c2,
									 const BoundingBox& bb,
									 const unsigned int width,
									 const unsigned int height)
	{
		/* Grid with the dimension of the bounding box initialized to 0 */
		short matrix_bbox[bb.m_W * bb.m_H];
		memset(matrix_bbox, 0, bb.m_W * bb.m_H * sizeof(short));
		
		/* Generate the border pixels wrt to c1 and c2 */
		PixelList pixels1 = GenerateBorderPixels(start_id1, c1, width);
		{
			PixelList pixels2 = GenerateBorderPixels(start_id2, c2, width);
			pixels1.insert(pixels1.end(), pixels2.begin(), pixels2.end());
		}

		/* Each case of the grid where a pixel is located is set to 1 */
		for(PixelConstIterator pit = pixels1.begin(); pit != pixels1.end(); ++pit)
			matrix_bbox[ToBoundingBoxCoordinates(*pit, bb, width)] = 1;

		/* Remove internal pixels */
		long unsigned int bb_id;
		bool is_internal;
		long int neighbors[8];
		for(long unsigned int y = 1; y < bb.m_H - 1; ++y)
		{
			for(long unsigned int x = 1; x < bb.m_W - 1; ++x)
			{
				bb_id = y*bb.m_W + x;
				is_internal = true;
				if(matrix_bbox[bb_id] > 0)
				{
					EIGHTNeighborhood(neighbors, bb_id, bb.m_W, bb.m_H);
					for(short j = 0; j < 8; ++j)
					{
						if(matrix_bbox[neighbors[j]] < 1)
						{
							is_internal = false;
							break;
						}
					}

					if(is_internal)
						matrix_bbox[bb_id] = 2;
				}
			}
		}

		for(long unsigned int i = 0; i < bb.m_H * bb.m_W; ++i)
		{
			if(matrix_bbox[i] > 1)
				matrix_bbox[i] = 0;
		}

		/* Create a new contour */
		return CreateContourFromBorderPixels(start_id1, bb, matrix_bbox, width);
		
	}

	long unsigned int
	ContourOperations::ToImageCoordinates(const long unsigned int id,
										  const BoundingBox& bbox,
										  const unsigned int width)
	{
		long unsigned int bbox_y = id / bbox.m_W;
		long unsigned int bbox_x = id % bbox.m_W;
		long unsigned int img_x = bbox.m_UX + bbox_x;
		long unsigned int img_y = bbox.m_UY + bbox_y;
		return (img_y * width + img_x);
	}

	typename ContourOperations::PixelList
	ContourOperations::GeneratePixels(long unsigned int id,
									  const Contour& contour,
									  const BoundingBox& bbox,
									  const unsigned int width)
	{
		// Generate the bounding box grid
		short matrix_bbox[bbox.m_W * bbox.m_H];
		memset(matrix_bbox,  0, bbox.m_W * bbox.m_H * sizeof(short));

		PixelList pixels;

		/* Each case of the grid where a pixel is located is set to 1 */
		{
			// Generate the border pixels
			PixelList border_pixels = GenerateBorderPixels(id, contour, width);
			
			for(PixelConstIterator pit = border_pixels.begin();
				pit != border_pixels.end(); ++pit)
				matrix_bbox[ToBoundingBoxCoordinates(*pit, bbox, width)] = 1;

			pixels.insert(pixels.end(), border_pixels.begin(), border_pixels.end());
		}

		/* For each case of the grid we determine if it is inside or outside
		   of the region */
		long unsigned int bb_id;
		for(long unsigned int y = 1; y < bbox.m_H - 1; ++y)
		{
			for(long unsigned int x = 1; x < bbox.m_W - 1; ++x)
			{
				bb_id = y*bbox.m_W + x;
				if(matrix_bbox[bb_id] < 1)
				{
					if(IsInternalPixel(bb_id, matrix_bbox, bbox.m_W, bbox.m_H))
					{
						matrix_bbox[bb_id] = 1;
						pixels.push_back(ToImageCoordinates(bb_id, bbox, width));
					}
				}
			}
		}
		return pixels;
	}
} // end of namespace lsrm
