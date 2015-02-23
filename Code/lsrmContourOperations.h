#ifndef __LSRM_CONTOUR_OPERATIONS_H
#define __LSRM_CONTOUR_OPERATIONS_H
#include "lsrmDataStructures.h"
#include "lsrmNeighborhood.h"
#include <cstring>
#include <algorithm>

namespace lsrm
{
	class ContourOperations
	{
	public:

		/* Some convenient typedefs */
		typedef std::vector<long unsigned int> PixelList;
		typedef typename PixelList::iterator PixelIterator;
		typedef typename PixelList::const_iterator PixelConstIterator;

		/*
		 * Given two rectangular bounding boxes,
		 * it returns the bounding box which is
		 * the union of both bounding boxes.
		 *
		 * @params
		 * const BoundingBox& bb1 : reference to the first bounding box
		 * const BoundingBox& bb2 : reference to the second bounding box
		 * @return the union of bb1 and bb2.
		 */
		static BoundingBox MergeBoundingBoxes(const BoundingBox& bb1,
											  const BoundingBox& bb2);

		/*
		 * Given the coordinates of the pixel in the current image, the
		 * bounding box containing it and the width of the image, it returns
		 * the coordinates of the pixel in the referential of the bounding box.
		 *
		 * @params
		 * const long unsigned int id : coordinates of the pixel in the image
		 *                              referential.
		 * const BoundingBox& bb : bounding box containing the pixel
		 * const unsigned int width : width of the image
		 * @return the coordinates of the pixel in the bounding box referential.
		 */
		static long unsigned int ToBoundingBoxCoordinates(const long unsigned int id,
												   const BoundingBox& bb,
												   const unsigned int width);


		/*
		 * Given the contour, the first pixel coordinates
		 * and the width of the current image,
		 * it returns the list of the border pixels.
		 *
		 * @params
		 * const long unsigned int id : coordinates of the first pixel.
		 * const Contour& contour : reference to the contour.
		 * const unsigned int width : width of the current image.
		 */
		static PixelList GenerateBorderPixels(long unsigned int id,
											  const Contour& contour,
											  const unsigned int width);

		/*
		 * Return true if it exists a pixel in the grid at the north
		 * of the current pixel.
		 *
		 * @params
		 * const long unsigned int idx : coordinates of the current pixel
		 * const short * grid : grid
		 * const unsigned int grid_width : width of the grid
		 */
		static bool GetCollisionAtNorth(const long unsigned int idx,
										const short * grid,
										const unsigned int grid_width);

		/*
		 * Return true if it exists a pixel in the grid at the north East
		 * of the current pixel.
		 *
		 * @params
		 * const long unsigned int idx : coordinates of the current pixel
		 * const short * grid : grid
		 * const unsigned int grid_width : width of the grid
		 */
		static bool GetCollisionAtNorthEast(const long unsigned int idx,
											const short * grid,
											const unsigned int grid_width);

		/*
		 * Return true if it exists a pixel in the grid at the East
		 * of the current pixel.
		 *
		 * @params
		 * const long unsigned int idx : coordinates of the current pixel
		 * const short * grid : grid
		 * const unsigned int grid_width : width of the grid
		 */
		static bool GetCollisionAtEast(const long unsigned int idx,
									   const short * grid,
									   const unsigned int grid_width);

		/*
		 * Return true if it exists a pixel in the grid at the South East
		 * of the current pixel.
		 *
		 * @params
		 * const long unsigned int idx : coordinates of the current pixel
		 * const short * grid : grid
		 * const unsigned int grid_width : width of the grid
		 * const unsigned int grid_height : height of the grid
		 */
		static bool GetCollisionAtSouthEast(const long unsigned int idx,
											const short * grid,
											const unsigned int grid_width,
											const unsigned int grid_height);

		/*
		 * Return true if it exists a pixel in the grid at the South
		 * of the current pixel.
		 *
		 * @params
		 * const long unsigned int idx : coordinates of the current pixel
		 * const short * grid : grid
		 * const unsigned int grid_width : width of the grid
		 * const unsigned int grid_height : height of the grid
		 */
		static bool GetCollisionAtSouth(const long unsigned int idx,
										const short * grid,
										const unsigned int grid_width,
										const unsigned int grid_height);

		/*
		 * Return true if it exists a pixel in the grid at the South West
		 * of the current pixel.
		 *
		 * @params
		 * const long unsigned int idx : coordinates of the current pixel
		 * const short * grid : grid
		 * const unsigned int grid_width : width of the grid
		 * const unsigned int grid_height : height of the grid
		 */
		static bool GetCollisionAtSouthWest(const long unsigned int idx,
											const short * grid,
											const unsigned int grid_width,
											const unsigned int grid_height);

		/*
		 * Return true if it exists a pixel in the grid at the West
		 * of the current pixel.
		 *
		 * @params
		 * const long unsigned int idx : coordinates of the current pixel
		 * const short * grid : grid
		 * const unsigned int grid_width : width of the grid
		 */
		static bool GetCollisionAtWest(const long unsigned int idx,
									   const short * grid,
									   const unsigned int grid_width);

		/*
		 * Return true if it exists a pixel in the grid at the North West
		 * of the current pixel.
		 *
		 * @params
		 * const long unsigned int idx : coordinates of the current pixel
		 * const short * grid : grid
		 * const unsigned int grid_width : width of the grid
		 */
		static bool GetCollisionAtNorthWest(const long unsigned int idx,
											const short * grid,
											const unsigned int grid_width);

		/*
		 * Return true if the pixel located at the coordinates idx
		 * is an internal pixel, i.e, in each direction there is a pixel.
		 *
		 * @params
		 * const long unsigned int idx : coordinates of the current pixel
		 * const short * grid : grid
		 * const unsigned int grid_width : width of the grid
		 * const unsigned int grid_height : height of the grid
		 */
		static bool IsInternalPixel(const long unsigned int idx,
									const short * grid,
									const unsigned int grid_width,
									const unsigned int grid_height);

		/*
		 * Given the direction of the front, it returns the field of view
		 *
		 * @params
		 * short * sight : former field of view.
		 * const short direction : new front direction.
		 */
		static void UpdateSight(short * sight, const short direction);


		/*
		 * Create a contour element when we move toward the top, right, bottom
		 * or left wrt to the current pixel.
		 *
		 * @params
		 * long unsigned int& curr_mat_id : pixel coordinates
		 * short& direction : toward the direction we look
		 * short& pos : location on the pixel
		 * Contour& curr_contour : new contour to be created
		 * const unsigned int grid_width : width of the grid
		 */
		static void EncodeContourForTopPixel(long unsigned int& curr_mat_id,
											 short& direction,
											 short& pos,
											 Contour& curr_contour,
											 const unsigned int grid_width);

		static void EncodeContourForRightPixel(long unsigned int& curr_mat_id,
											   short& direction,
											   short& pos,
											   Contour& curr_contour);

		static void EncodeContourForBottomPixel(long unsigned int& curr_mat_id,
												short& direction,
												short& pos,
												Contour& curr_contour,
												const unsigned int grid_width);

		static void EncodeContourForLeftPixel(long unsigned int& curr_mat_id,
											  short& direction,
											  short& pos,
											  Contour& curr_contour);
		
		/*
		 * Given the coordinates of the first pixel, the bounding box, the grid
		 * with the location of the border pixels and the dimension of the referential
		 * image, it returns a new contour
		 *
		 * @params
		 * const long unsigned int idx : coordinates of the current pixel.
		 * const BoundingBox& bbox : reference to the bounding box
		 * const short * grid : grid
		 * const unsigned int width : width of the current image.
		 * cont unsigned int height : height of the current image.
		 */
		static Contour CreateContourFromBorderPixels(const long unsigned int id,
													 const BoundingBox& bbox,
													 const short * grid,
													 const unsigned int width);
		

		/*
		 * Given two contours, the coordinates of their first pixels
		 * and the bounding box wrapping both contours, it returns the
		 * new contour by merging c1 and c2.
		 *
		 * @params
		 * const long unsigned int start_id1 : coordinates of the first
		 *                                     pixel of the first contour
		 * const long unsigned int start_id2 : coordinates of the first
		 *                                     pixel of the second contour
		 * const Contour& c1 : reference to the first contour
		 * const Contour& c2 : reference to the second contour
		 * const BoundingBox& bb : reference to the bounding box wrapping c1 and c2
		 * const unsigned int width : width of the current image
		 * const unsigned int height :  height of the current image
		 */
		static Contour MergeContours(const long unsigned int start_id1,
									 const long unsigned int start_id2,
									 const Contour& c1,
									 const Contour& c2,
									 const BoundingBox& bb,
									 const unsigned int width,
									 const unsigned int height);

		/*
		 * Given the coordinates of the pixel within its bounding box, it
		 * returns the coordinates of this pixel in the current image.
		 *
		 * @params
		 * const long unsigned int id : coordinates of the pixel within the bounding box
		 * const BoundingBox& bbox : reference to the bounding box
		 * const unsigned int width : width of the image
		 * @return the coordinates of the pixel in the image. 
		 */
		static long unsigned int ToImageCoordinates(const long unsigned int id,
													const BoundingBox& bbox,
													const unsigned int width);

		/*
		 * Given the contour, the first pixel coordinates
		 * and the width of the current image,
		 * it returns the list of the pixels within the area
		 * delineated by the contour.
		 *
		 * @params
		 * const long unsigned int id : coordinates of the first pixel.
		 * const Contour& contour : reference to the contour.
		 * const unsigned int width : width of the current image.
		 */
		static PixelList GeneratePixels(long unsigned int id,
										const Contour& contour,
										const BoundingBox& bbox,
										const unsigned int width);
	};
} // end of namespace lsrm

#endif
