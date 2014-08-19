# Introduction #

This project implements a Geometry Wars clone for educational and example purposes. The idea is to provide a concrete example of a 'traditional' game implementation designed around a model of the world, and then show how the code might change to achieve better performance and testability by examining the data first.

Geometry Wars was selected as an example game because it doesn't require much in the way of static assets (so no asset baking preprocess is required) and because the game deals with large numbers of enemies being active at once.

# Dependencies #

This project depends on the latest GLFW3, available from http://www.glfw.org, and on the latest GLEW, available from http://glew.sourceforge.net.

GLFW 3.0.4 and GLEW 1.10.0 compiled for OSX are included in this project.

OpenAL is required for audio playback. This is included on OSX, but requires a separate install on Windows.

This game has been tested on a MacBook Pro Retina 2.7GHz Core i7 with 16GB RAM and an NVIDIA GeForce 650M running OSX 10.9.4. OpenGL 4 support is required.

# License #

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this software, either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this software dedicate any and all copyright interest in the software to the public domain. We make this dedication for the benefit of the public at large and to the detriment of our heirs and successors. We intend this dedication to be an overt act of relinquishment in perpetuity of all present and future rights to this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
