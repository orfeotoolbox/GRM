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
#ifndef __LP_CONTOUR_H
#define __LP_CONTOUR_H
#include <cassert>
#include <iostream>
#include <bitset>
#include <vector>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <boost/dynamic_bitset.hpp>

// Improvement: do not use a boolean matrix but a collection because
// at the end of the process matrices are sparses.

namespace lp
{
	/* Move along a contour: 4 possibles: top (0), right(1), bottom (2), left(3). */
	using Move = std::bitset<2>;
	/* A contour is a list of moves represented by a pair of bits*/
	using Contour = boost::dynamic_bitset<>;
 
	using ContourIndex = boost::dynamic_bitset<>::size_type;

	/* Index of a cell in the grid */
	using CellIndex = std::size_t;

	/* List of cell indices */
	using CellLists = std::unordered_set<CellIndex>;
	
	struct BoundingBox
	{
		std::size_t m_UX;
		std::size_t m_UY;
		std::size_t m_W;
		std::size_t m_H;
	};

	class ContourOperations
	{
	public:
		/* Methods to fill a contour. */
		static void Push0(Contour& contour); // Push a move to the top.
		static void Push1(Contour& contour); // Push a move to the right.
		static void Push2(Contour& contour); // Push a move to the bottom.
		static void Push3(Contour& contour); // Push a move to the left.
		
		/* Methods to access to elements of a contour. */
		static Move GetMove2(ContourIndex idx, const Contour& contour); // Retrieve a move
		static short GetMove10(const Move& m); // Transform the move into a 10 base number (0,1,2 or 3).

		/* Main methods */
		static void MergeContour(Contour& mergedContour,
								 BoundingBox& mergedBoundingBox,
								 Contour& contour1,
								 Contour& contour2,
								 BoundingBox& bbox1,
								 BoundingBox& bbox2,
								 const CellIndex cid1,
								 const CellIndex cid2,
								 const std::size_t gridSizeX);

		static void GenerateBorderCellsForContourFusion(CellLists& borderCells, 
														const Contour& contour, 
														const CellIndex startCellId, 
														const std::size_t gridSizeX,
														const BoundingBox& mergedBBox);
		
		static void GenerateBorderCells(CellLists& borderCells, // From a contour, the smallest id of the cell
										const Contour& contour, // belonging to the shape and the size of the grid,
										const CellIndex startCellId, // this method returns the ids of the border cells
										const std::size_t gridSizeX); // of the shape.

		static void CreateNewContour(Contour& newContour,
									 const CellIndex startCellId,
									 CellLists& setCells,
									 const std::size_t bboxWidth,
									 const std::size_t bboxHeight);

		/* Utilities methods */
		static BoundingBox MergeBoundingBoxes(const BoundingBox& bb1, // Given 2 bounding boxes return the bounding
											  const BoundingBox& bb2); // which is the union.

		static void EIGHTNeighborhood(long int * neighborhood, // return 8-neighbors of cell id.
									  const CellIndex id,
									  const std::size_t width,
									  const std::size_t height);

		static CellIndex BBoxToGrid(const CellIndex bboxId,
									const BoundingBox& bbox,
									const std::size_t gridSizeX);

		static CellIndex GridToBBox(const CellIndex gridId, // Return the coordinates of the cell
									const BoundingBox& bbox, // in the bounding box reference.
									const std::size_t gridSizeX);
	};
} // end of namespace lp
#endif









