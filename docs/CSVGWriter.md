# CSVGWriter Explanation

## Overview

CSVGWriter provides a C++ interface for generating SVG elements. its a wrapper for the C SVG library that I created in Project 1 for drawing shapes and grouping like elements. It produces SVG text that is valid to write them to the Data Sink.

## Key functions

automatically writes SVG start and ends
supports the creation of circles, rectangles, lines, and paths (from the svg.h library)
groups elements of similar tags
uses attributes to generate style strings to be put into the tags