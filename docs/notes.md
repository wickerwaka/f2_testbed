TC0110SCN RAM read states
BG0 Code, BG0 Attrib, Col Scroll, FG0 Gfx, BG1 Code, BG1 Attrib, FG0 Next, CPU

At the start of each line it reads
BG0 Rowscroll, Ignored?, Ignored?, Ignored?, BG1 Rowscroll, Ignored?, Ignored?, CPU


OLDH seems to indicate the start of each line. OLDV likely the same thing for vertical. 


TC0200OBJ shares it's buffer memory with the CPU. All the DMA phase does is store a copy of the position information into two unused words in the 8-word instance definition. This seems to allow the CPU to make changes to positions while sprite rendering is also happening. Supports double buffer modes too, but finalblow doesn't use it.


