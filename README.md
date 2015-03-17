topomodel
======

topomodel is a simple tool that generates a surface slope and aspect from a set of  
x, y, z floating point binary grids and a byte input mask. The gradients are computed 
via 3rd order finite difference weighted by the reciprocal of the squared distance.
The kernel = standard 3, and the slope output is degrees from horizontal, the aspect 
output is degrees from the north, e.g. n=>0|360, e=>90, s=>180, w=>270.

For the purposes of this program a grid is defined as: 

```
Upper left (x,y)
    +-------------+----------------------------------+
    | element 0,0 | .  .  .  .  .  .  .  .  .  .  .  |
    +-------------+                                  |   
    |     .                                          |   
    |     .                                          |   
    |     .                                          |   
    |     .                         +----------------|
    |     .                         | element nx, ny |
    +-------------------------------+----------------+
                                              lower right (x,y)
```
Note that this tool is used for automating the surface generation process over
an arbitrary x,y,z,m large set.

See the topomodel man pages for more information.

## References:
Horn, B.K.P., 1981. Hill shading and the reflectance map. Proc. Inst. Electric. Electron. Eng. 69, 14–47.
