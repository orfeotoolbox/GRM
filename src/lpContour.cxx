/*=========================================================================

  Program: Contour Library LP
  Language: C++
  author: Lassalle Pierre
  contact: lassallepierre34@gmail.com



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "lpContour.h"

namespace lp
{
	
	void ContourOperations::MergeContour(Contour& mergedContour,
										 BoundingBox& mergedBBox,
										 Contour& contour1,
										 Contour& contour2,
										 BoundingBox& bbox1,
										 BoundingBox& bbox2,
										 const CellIndex cid1,
										 const CellIndex cid2,
										 const std::size_t gridSizeX)
	{
		// First step consists of building the bounding box resulting from the fusion
		// of the bounding boxes bbox1 and bbox2
		mergedBBox = MergeBoundingBoxes(bbox1, bbox2);

		// Create the associate matrix indicating where the cells are located
		// inside the bbox
		std::vector<bool> cellMatrix(mergedBBox.m_W*mergedBBox.m_H, false);
		std::unordered_set<CellIndex> setCells;

		// Fill the cell matrix with the cells from both contours
		CellLists borderCells;

        // Fill with the cells of contour 1
		GenerateBorderCellsForContourFusion(borderCells, contour1, cid1, gridSizeX, mergedBBox);
		// Fill with the cells of contour 2
		GenerateBorderCellsForContourFusion(borderCells, contour2, cid2, gridSizeX, mergedBBox);

		// Create the new contour
		CreateNewContour(mergedContour, GridToBBox(cid1, mergedBBox, gridSizeX),  borderCells, mergedBBox.m_W, mergedBBox.m_H);
	}


	void ContourOperations::CreateNewContour(Contour& newContour,
											 const CellIndex cidx,
											 CellLists& setCells,
											 const std::size_t bboxWidth,
											 const std::size_t bboxHeight)
	{
		// The first move is always 1
		Push1(newContour);

		// Previous move
		short prevMove = 1;

		// Local pixel id
		CellIndex currIdx = cidx;

		// Table containing id neighbors
		long int neighbors[8];

		for(;;)
		{
		
			// Compute neighbor'ids
			EIGHTNeighborhood(neighbors, currIdx, bboxWidth, bboxHeight);
			
			if(prevMove == 1)
			{
				if(neighbors[1] != -1 && setCells.find(neighbors[1]) != setCells.end())
				{
					Push0(newContour);
					currIdx = currIdx + 1 - bboxWidth;
					prevMove = 0;
				}
				else if(neighbors[2] != -1 && setCells.find(neighbors[2]) != setCells.end())
				{
					Push1(newContour);
					currIdx++;
					prevMove = 1;
				}
				else
				{
					Push2(newContour);
					prevMove = 2;
				}
			}
			else if(prevMove == 2)
			{
				if(neighbors[3] != -1 && setCells.find(neighbors[3]) != setCells.end()) 
				{
					Push1(newContour);
					currIdx = currIdx + bboxWidth + 1;
					prevMove = 1;
				}
				else if(neighbors[4] != -1 && setCells.find(neighbors[4]) != setCells.end()) 
				{
					Push2(newContour);
					currIdx += bboxWidth;
					prevMove = 2;
				}
				else
				{
					Push3(newContour);
					prevMove = 3;
				}
			}
			else if(prevMove == 3)
			{
			
				if(neighbors[5] != -1 && setCells.find(neighbors[5]) != setCells.end())
				{
					Push2(newContour);
					currIdx = currIdx - 1 + bboxWidth;
					prevMove = 2;
				}
				else if(neighbors[6] != -1 && setCells.find(neighbors[6]) != setCells.end())
				{
					Push3(newContour);
					currIdx -= 1;
					prevMove = 3;
				}
				else
				{
					Push0(newContour);
					prevMove = 0;
				}
			}
			else
			{
				assert(prevMove == 0);

				if(neighbors[7] != -1 && setCells.find(neighbors[7]) != setCells.end())
				{
					Push3(newContour);
					currIdx = currIdx - bboxWidth - 1;
					prevMove = 3;
				}
				else if(neighbors[0] != -1 && setCells.find(neighbors[0]) != setCells.end())
				{
					Push0(newContour);
					currIdx -= bboxWidth;
					prevMove = 0;
				}
				else
				{
					if(currIdx == cidx)
						break;
					else
					{
						Push1(newContour);
						prevMove = 1;
					}
				}
			}
		}
	}

	void ContourOperations::GenerateBorderCellsForContourFusion(CellLists& borderCells, 
																const Contour& contour, 
																const CellIndex startCellId, 
																const std::size_t gridSizeX,
																const BoundingBox& mergedBBox)
	{
		// Add the first pixel to the border list
		borderCells.insert(GridToBBox(startCellId, mergedBBox, gridSizeX));

		if(contour.size() > 8)
		{
	
			// Intialize the first move at prev
			short curr, prev = GetMove10(GetMove2(0, contour));

			// Declare the current pixel index
			CellIndex idx = startCellId;

			// Explore the contour
			for(ContourIndex cidx = 1; cidx < contour.size() / 2; cidx++)
			{
				curr = GetMove10(GetMove2(cidx, contour));
				assert(curr >= 0 && curr < 4);

				if(curr == 0)
				{
					// Impossible case is prev = 2;
					assert(prev != 2);

					//*
					//*
					if(prev == 0)
					{
						idx -= gridSizeX;
						borderCells.insert(GridToBBox(idx, mergedBBox, gridSizeX));
					}
			
					//  *
					// **
					if(prev == 1)
					{
						idx = idx + 1 - gridSizeX;
						borderCells.insert(GridToBBox(idx, mergedBBox, gridSizeX));
					}
				}else if(curr == 1)
				{
					// Impossible case
					assert(prev != 3);

					// **
					if(prev == 1)
					{
						idx++;
						borderCells.insert(GridToBBox(idx, mergedBBox, gridSizeX));
					}

					//*
					//**
					if (prev == 2)
					{
						idx = idx + 1 + gridSizeX;
						borderCells.insert(GridToBBox(idx, mergedBBox, gridSizeX));
					}
			
				}else if(curr == 2)
				{
					// Impossible case
					assert(prev != 0);

					// *
					// *
					if(prev == 2)
					{
						idx += gridSizeX;
						borderCells.insert(GridToBBox(idx, mergedBBox, gridSizeX));
					}

					// **
					// *
					if(prev == 3)
					{
						idx = idx - 1 + gridSizeX;
						borderCells.insert(GridToBBox(idx, mergedBBox, gridSizeX));
					}
				}else
				{
					// Impossible case
					assert(prev != 1);

					if(prev == 0)
					{
						idx = idx - 1 - gridSizeX;
						borderCells.insert(GridToBBox(idx, mergedBBox, gridSizeX));
					}

					if(prev == 3)
					{
						idx--;
						borderCells.insert(GridToBBox(idx, mergedBBox, gridSizeX));
					}
				}

				prev = curr;
			}
		}
	}
	
	void ContourOperations::GenerateBorderCells(CellLists& borderCells, 
												const Contour& contour, 
												const CellIndex startCellId, 
												const std::size_t gridSizeX)
	{
		// Add the first pixel to the border list
		borderCells.insert(startCellId);

		if(contour.size() > 8)
		{
	
			// Intialize the first move at prev
			short curr, prev = GetMove10(GetMove2(0, contour));

			// Declare the current pixel index
			CellIndex idx = startCellId;

			// Explore the contour
			for(ContourIndex cidx = 1; cidx < contour.size() / 2; cidx++)
			{
				curr = GetMove10(GetMove2(cidx, contour));
				assert(curr >= 0 && curr < 4);

				if(curr == 0)
				{
					// Impossible case is prev = 2;
					assert(prev != 2);

					//*
					//*
					if(prev == 0)
					{
						idx -= gridSizeX;
						borderCells.insert(idx);
					}
			
					//  *
					// **
					if(prev == 1)
					{
						idx = idx + 1 - gridSizeX;
						borderCells.insert(idx);
					}
				}else if(curr == 1)
				{
					// Impossible case
					assert(prev != 3);

					// **
					if(prev == 1)
					{
						idx++;
						borderCells.insert(idx);
					}

					//*
					//**
					if (prev == 2)
					{
						idx = idx + 1 + gridSizeX;
						borderCells.insert(idx);
					}
			
				}else if(curr == 2)
				{
					// Impossible case
					assert(prev != 0);

					// *
					// *
					if(prev == 2)
					{
						idx += gridSizeX;
						borderCells.insert(idx);
					}

					// **
					// *
					if(prev == 3)
					{
						idx = idx - 1 + gridSizeX;
						borderCells.insert(idx);
					}
				}else
				{
					// Impossible case
					assert(prev != 1);

					if(prev == 0)
					{
						idx = idx - 1 - gridSizeX;
						borderCells.insert(idx);
					}

					if(prev == 3)
					{
						idx--;
						borderCells.insert(idx);
					}
				}

				prev = curr;
			}
		}
	}

	CellIndex ContourOperations::BBoxToGrid(const CellIndex bboxId,
											const BoundingBox& bbox,
											const std::size_t gridSizeX)
	{
		CellIndex bbX = bboxId % bbox.m_W;
		CellIndex bbY = bboxId / bbox.m_W;

		CellIndex gridX = bbox.m_UX + bbX;
		CellIndex gridY = bbox.m_UY + bbY;

		CellIndex gridId = gridY * gridSizeX + gridX;

		return gridId;
	}

	CellIndex ContourOperations::GridToBBox(const CellIndex gridId,
											const BoundingBox& bbox,
											const std::size_t gridSizeX)
	{
		CellIndex gridX = gridId % gridSizeX;
		CellIndex gridY = gridId / gridSizeX;

		CellIndex bbX = gridX - bbox.m_UX;
		CellIndex bbY = gridY - bbox.m_UY;

		CellIndex bbId = bbY * bbox.m_W + bbX;

		return bbId;
	}
	
	void ContourOperations::EIGHTNeighborhood(long int * neighborhood,
											  const CellIndex id,
											  const std::size_t width,
											  const std::size_t height)
	{
		const unsigned int x = id % width;
		const unsigned int y = id / width;

		/* top */
		neighborhood[0] = ( y > 0 ? (id - width) : -1 );

		/* top right */
		neighborhood[1] = ( (y > 0 && x < (width - 1) ) ? (id - width + 1) : -1 );
		
		/* right */
		neighborhood[2] = ( x < (width - 1) ? (id + 1) : -1 );

		/* bottom right */
		neighborhood[3] = ( (x < (width - 1) && y < (height - 1) ) ? (id + 1 + width) : -1);

		/* bottom */
		neighborhood[4] = ( y < (height - 1) ? (id + width) : -1 );

		/* bottom left */
		neighborhood[5] = ( (y < (height - 1) && x > 0) ? (id + width - 1) : -1 );
		
		/* left */
		neighborhood[6] = ( x > 0 ? (id - 1) : -1 );

		/* top left */
		neighborhood[7] = ( (x > 0 && y > 0) ? (id -width - 1) : - 1);	
	}
	
	BoundingBox ContourOperations::MergeBoundingBoxes(const BoundingBox& bb1,
													  const BoundingBox& bb2)
	{
		std::size_t min_ux, min_uy, max_xw, max_yh;
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
	
	Move ContourOperations::GetMove2(ContourIndex idx, const Contour& contour)
	{
		return Move(2*contour[2*idx] + contour[idx*2 +1]);
	}

	short ContourOperations::GetMove10(const Move& m)
	{
		return (m[0]*2 + m[1]);
	}
	
	void ContourOperations::Push0(Contour& contour)
	{
		contour.push_back(0); contour.push_back(0);
	}

	void ContourOperations::Push1(Contour& contour)
	{
		contour.push_back(1); contour.push_back(0);
	}

	void ContourOperations::Push2(Contour& contour)
	{
		contour.push_back(0); contour.push_back(1);
	}

	void ContourOperations::Push3(Contour& contour)
	{
		contour.push_back(1); contour.push_back(1);
	}
} // end of namespace lp
