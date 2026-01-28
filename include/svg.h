/**
 * @file svg.h
 * @brief Simple SVG drawing interface.
 *
 * Provides basic types and functions for creating SVG documents.
 */

#ifndef SVG_H
#define SVG_H

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Opaque SVG drawing context.
 *
 * Holds internal state required to generate an SVG document.
 * The contents of this structure are intentionally hidden
 * from the user.
 */
typedef struct SVG_CONTEXT svg_context_t;

/**
 * @brief Pointer to an SVG drawing context.
 */
typedef svg_context_t *svg_context_ptr;

/**
 * @brief Return type used by SVG API functions.
 *
 * A value of zero typically indicates success, while non-zero
 * values indicate an error condition.
 */
typedef enum {
    SVG_OK = 0,            /**< Operation succeeded */
    SVG_ERR_NULL,          /**< NULL pointer passed */
    SVG_ERR_IO,            /**< Write callback failed */
    SVG_ERR_INVALID_ARG,   /**< Invalid parameter value */
    SVG_ERR_STATE          /**< Invalid context state */
} svg_return_t;

/**
 * @brief User-defined context pointer.
 *
 * Passed unchanged to user-supplied callback functions.
 */
typedef void *svg_user_context_ptr;

/**
 * @brief Pixel-based dimension type.
 *
 * Used for canvas width and height, which are discrete values.
 */
typedef int svg_px_t;

/**
 * @brief Real-number type used for SVG coordinates and lengths.
 *
 * Abstracts the underlying floating-point representation.
 */
typedef double svg_real_t;

/**
 * @brief Coordinate type in SVG space.
 *
 * Represents a single coordinate value (x or y).
 */
typedef svg_real_t svg_coord_t;

/**
 * @brief Two-dimensional point in SVG coordinate space.
 */
typedef struct{
    svg_coord_t x;  /**< X coordinate */
    svg_coord_t y;  /**< Y coordinate */
} svg_point_t;

/**
 * @brief Two-dimensional size in SVG coordinate space.
 */
typedef struct{
    svg_coord_t width;  /**< X length */
    svg_coord_t height; /**< Y length */
} svg_size_t;

/**
 * @brief Callback used to write SVG output.
 *
 * Called by the SVG context whenever text output is required.
 *
 * @param user User-defined context pointer
 * @param text Null-terminated SVG text
 *
 * @return Status code indicating success or failure
 */
typedef svg_return_t (*svg_write_fn)(svg_user_context_ptr user,
                                    const char *text);

/**
 * @brief Callback used to clean up user resources.
 *
 * Called when the SVG context is destroyed.
 *
 * @param user User-defined context pointer
 *
 * @return Status code indicating success or failure
 */
typedef svg_return_t (*svg_cleanup_fn)(svg_user_context_ptr user);

/**
 * @brief Creates a new SVG drawing context.
 *
 * Initializes an SVG context with the given canvas dimensions and user-defined
 * output callbacks.
 *
 * @param write_fn   Callback used to write SVG text output
 * @param cleanup_fn Callback used to clean up user resources
 * @param user       User-defined context passed to callbacks
 * @param width      Canvas width in pixels
 * @param height     Canvas height in pixels
 *
 * @return Pointer to a newly created SVG context, or NULL on failure
 */
svg_context_ptr svg_create(svg_write_fn write_fn, 
                           svg_cleanup_fn cleanup_fn, 
                           svg_user_context_ptr user, 
                           svg_px_t width, 
                           svg_px_t height);

/**
 * @brief Destroys an SVG context.
 *
 * Finalizes the SVG output and releases all resources associated
 * with the context.
 *
 * @param context SVG context to destroy
 *
 * @return Status code indicating success or failure
 */
svg_return_t svg_destroy(svg_context_ptr context);

/**
 * @brief Draws a circle.
 *
 * Writes an SVG <circle> element using the given center point and radius.
 *
 * @param context SVG context to draw into
 * @param center  Center point of the circle
 * @param radius  Circle radius
 * @param style   SVG style string (may be NULL)
 *
 * @return Status code indicating success or failure
 */
svg_return_t svg_circle(svg_context_ptr context,
                        const svg_point_t *center,
                        svg_real_t radius,
                        const char* style);

/**
 * @brief Draws a rectangle.
 *
 * Writes an SVG <rect> element using the given top-left point and dimensions.
 *
 * @param context  SVG context to draw into
 * @param top_left Top-left corner of the rectangle
 * @param widt     Rectangle width
 * @param height   Rectangle height
 * @param style    SVG style string (may be NULL)
 *
 * @return Status code indicating success or failure
 */
svg_return_t svg_rect(svg_context_ptr context,
                      const svg_point_t *top_left,
                      const svg_size_t *size,
                      const char *style);

/**
 * @brief Draws a line segment.
 *
 * Writes an SVG <line> element from the start point to the end point.
 *
 * @param context SVG context to draw into
 * @param start   Start point of the line
 * @param end     End point of the line
 * @param style   SVG style string (may be NULL)
 *
 * @return Status code indicating success or failure
 */
svg_return_t svg_line(svg_context_ptr context,
                      const svg_point_t *start,
                      const svg_point_t *end,
                      const char *style);

/**
 * @brief Begins an SVG group.
 *
 * Writes an opening <g> tag with the given attributes.
 * All subsequent elements will be part of this group
 * until svg_group_end() is called.
 *
 * @param context SVG context to draw into
 * @param attrs   SVG attribute string for the group (may be NULL)
 *
 * @return Status code indicating success or failure
 */
svg_return_t svg_group_begin(svg_context_ptr context, 
                             const char *attrs);

/**
 * @brief Ends the current SVG group.
 *
 * Writes a closing </g> tag.
 *
 * @param context SVG context to draw into
 *
 * @return Status code indicating success or failure
 */
svg_return_t svg_group_end(svg_context_ptr context);

#ifdef __cplusplus
}
#endif

#endif
