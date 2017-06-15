This is a test of a clickmap system that uses the stencil buffer.

The intent is to be able to render any kind of complex widget(s) and get pixel-perfect mouse picking.

Each object that can be picked draws with its own unique stencil value and the stencil buffer is sampled to determine which item is at a specific pixel coordinate.

There's a little more to this concept, but this is only a test to see what the performance impact of downloading the stencil buffer will be, so this is actually pessimized quite a bit, but the results (at least on the machines I tested it on) were promising.

The next stage in exploring this idea will be to determine what the actual cost (in ms) of the download is and try the same method in situations that are already pushing the limit of the hardware, then look at ways of limiting the impact (by downloading only when relevant changes have happened, for example).
