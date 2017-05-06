This is a test of a clickmap system that uses the stencil buffer.

The intent is to be able to render any kind of complex widget(s) and get pixel-perfect mouse picking.

Each object that can be picked draws with its own unique stencil value and the stencil buffer is sampled to determine which item is at a specific pixel coordinate.

There's a little more to this concept, but this is only a test to see what the performance impact of downloading the stencil buffer will be.
