/**
 * @file svg.c
 * @brief Implementation of simple SVG drawing interface.
 *
 * Implements the basic functions for creating SVG documents.
 */
#include "svg.h"
#include <stdlib.h>
#include <stdio.h>


/**
 * @brief Opaque SVG drawing context.
 *
 * Holds the necessary data to implement functions.
 */
struct SVG_CONTEXT{
    svg_write_fn write_fn;
    svg_cleanup_fn cleanup_fn;
    svg_user_context_ptr user;
    svg_px_t width;
    svg_px_t height;
    int depth;
};


svg_context_ptr svg_create(svg_write_fn write_fn, 
                           svg_cleanup_fn cleanup_fn, 
                           svg_user_context_ptr user, 
                           svg_px_t width, 
                           svg_px_t height)
{
    if(write_fn == NULL || cleanup_fn == NULL){
        return NULL;
    }

    if(width <= 0 || height <= 0){
        return NULL;
    }

    svg_context_ptr context = (svg_context_ptr)malloc(sizeof(svg_context_t));
    if(context == NULL){
        return NULL;
    }

    context->write_fn = write_fn;
    context->cleanup_fn = cleanup_fn;
    context->user = user;
    context->width = width;
    context->height = height;
    context->depth = 0;

    svg_return_t r = context->write_fn((context->user), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    if(r != SVG_OK){
        free(context);
        return NULL;
    }

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\">\n", width, height);

    r = context->write_fn(context->user, buffer);
    if(r != SVG_OK){
        free(context);
        return NULL;
    }


    return context;
}

svg_return_t svg_destroy(svg_context_ptr context){
    // Enter code here

    if(context == NULL){
        return SVG_ERR_NULL;
    }

    while(context->depth > 0){
        svg_return_t gr = context->write_fn(context->user, " </g>\n");
        if(gr != SVG_OK){
            context->cleanup_fn(context->user);
            free(context);
            return SVG_ERR_IO;
        }
        context->depth--;
    }

    svg_return_t r = context->write_fn(context->user, "</svg>\n");
    if(r != SVG_OK){
        context->cleanup_fn(context->user);
        free(context);
        return SVG_ERR_IO;
    }

    r = context->cleanup_fn(context->user);
    if(r != SVG_OK){
        free(context);
        return r;
    }

    free(context);
    return SVG_OK;

}

svg_return_t svg_circle(svg_context_ptr context,
                        const svg_point_t *center,
                        svg_real_t radius,
                        const char *style){
    // Enter code here

    //want to add the following xml code to the SVG file --TA Notes
    //<circle cx="50.000000" cy="50.000000" r="45.000000" style="fill:none; stroke:green; stroke-width:2"/>
    /*
    we are given our x and y as a svg_point_t, radius as a svg_real_t, and and want to use those
    we also want to use the provided style string, given that it is NOT NULL. if it is null, don’t use it!
    --TA Notes

    */

    if(context == NULL || center == NULL){
        return SVG_ERR_NULL;
    }
    if(radius < 0){
        return SVG_ERR_INVALID_ARG;
    }

    char buffer[512];

    if(style != NULL){
        snprintf(buffer, sizeof(buffer),
                 "  <circle cx=\"%f\" cy=\"%f\" r=\"%f\" style=\"%s\"/>\n",
                 center->x, center->y, radius, style);
    }else{
        snprintf(buffer, sizeof(buffer),
                 "  <circle cx=\"%f\" cy=\"%f\" r=\"%f\"/>\n",
                 center->x, center->y, radius);
    }

    svg_return_t r = context->write_fn(context->user, buffer);
    if(r != SVG_OK){
        return SVG_ERR_IO;
    }
    return SVG_OK;

}


svg_return_t svg_rect(svg_context_ptr context,
                      const svg_point_t *top_left,
                      const svg_size_t *size,
                      const char* style){
    // Enter code here
/*
for this function we want to write the following xml code to our SVG file:

<rect x="50.000000" y="50.000000" width="30.000000" height="40.000000" style="fill:none; stroke:green; stroke-width:2"/>

we are given our x and y as a svg_point_t, width and height as a svg_size_t, and want to use those

we also want to use the provided style string, given that it is NOT NULL. if it is null, don’t use it!
--TA Notes
*/

    if(context == NULL || top_left == NULL || size == NULL){
        return SVG_ERR_NULL;
    }
    if(size->width < 0 || size->height < 0){
        return SVG_ERR_INVALID_ARG;
    }

    char buffer[512];

    if(style != NULL){
        snprintf(buffer, sizeof(buffer),
                 "  <rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" style=\"%s\"/>\n",
                 top_left->x, top_left->y, size->width, size->height, style);
    }else{
        snprintf(buffer, sizeof(buffer),
                 "  <rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\"/>\n",
                 top_left->x, top_left->y, size->width, size->height);
    }

    svg_return_t r = context->write_fn(context->user, buffer);
    if(r != SVG_OK){
        return SVG_ERR_IO;
    }
    return SVG_OK;




}

svg_return_t svg_line(svg_context_ptr context,
                      const svg_point_t *start,
                      const svg_point_t *end,
                      const char* style){
    // Enter code here
    /*
    for this function we want to write the following xml code to our SVG file:

    <line x1="10.000000" y1="10.000000" x2="90.000000" y2="90.000000" style="stroke:green; stroke-width:2"/>

    we are given our x1 and y1 as a svg_point_t, x2 and y2 as a svg_point_t, and want to use those

    we also want to use the provided style string, given that it is NOT NULL. if it is null, don’t use it!

    --TA Notes
    */

    if(context == NULL || start == NULL || end == NULL){
        return SVG_ERR_NULL;
    }

    char buffer[512];

    if(style != NULL){
        snprintf(buffer, sizeof(buffer),
                 "  <line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"%s\"/>\n",
                 start->x, start->y, end->x, end->y, style);
    }else{
        snprintf(buffer, sizeof(buffer),
                 "  <line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\"/>\n",
                 start->x, start->y, end->x, end->y);
    }

    svg_return_t r = context->write_fn(context->user, buffer);
    if(r != SVG_OK){
        return SVG_ERR_IO;
    }
    return SVG_OK;

}

svg_return_t svg_group_begin(svg_context_ptr context, 
                             const char* attrs){
    // Enter code here
    /*
    for this function we want to write the following xml code to our SVG file:

    <g stroke:blue; stroke-width:2>

    we are given our attributes as a whole string and want to use those, given that the string is NOT NULL!

    --TA Notes
    */

    if(context == NULL){
        return SVG_ERR_NULL;
    }

    char buffer[512];
    if(attrs != NULL){
        snprintf(buffer, sizeof(buffer), "  <g %s>\n", attrs);
    }else{
        snprintf(buffer, sizeof(buffer), "  <g>\n");
    }

    svg_return_t r = context->write_fn(context->user, buffer);
    if(r != SVG_OK){
        return SVG_ERR_IO;
    }

    context->depth++;
    return SVG_OK;


}

svg_return_t svg_group_end(svg_context_ptr context){
    // Enter code here
    //and finally, for this function we simply want to add a closing </g> tag to our SVG file -- TA Notes

    if(context == NULL){
        return SVG_ERR_NULL;
    }
    if(context->depth <= 0){
        return SVG_ERR_STATE;
    }

    svg_return_t r = context->write_fn(context->user, "  </g>\n");
    if(r != SVG_OK){
        return SVG_ERR_IO;
    }

    context->depth--;
    return SVG_OK;

}

