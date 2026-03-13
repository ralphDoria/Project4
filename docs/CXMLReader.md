# CXMLReader Explanation

## overview

CXMLReader is a wrapper around and XML parser for reading XML Data. instead of using the parser callbacks, it iterates through XML entities, like a start tag or char data one at a time

## key functions

1. Uses a queue to store XML entities that were already parsed
2. supports start elements, char data, end elements, and complete elements
3. handles long char data over the byte limit
4. has an End() function which detects when all XML data has already been parsed