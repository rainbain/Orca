# CP In My Own Words
The CP or Command Processor is a key feature to the gamecubes graphics operations. Its through the CP that 
the CPU can acess the XF, BP, and other components memory. The CP also manages vertexs and vertex attributes. 
An arangement of vertx attributes are compiled together into the inputs of the XF. 

Last Updated 7/11/2023

All this is derived from https://www.gc-forever.com/yagcd/ and the Dolphin Emulator Source Code


## Opcodes
OpCodes come into the CP as 8 bit commands. The data read after is dependent on the command. 
Each OpCode has a very specific function with a lot of gotchas, so it should be well described here. 

### 0x00: No Operation
This command does nothing, but is intresting. A NOP command takes 1 cycle to execute. This hints that 
its the beggining of the CPs state system, and more like an IDLE. 

### 0x08: CP Load
This command loads the internal registers of the CP with a 32 bit value. 
The first argument is a 8 bit address. The second is a 32 bit value. 
When using this command theres a few important notes: 
 * Some of the CPs registers only use the up 4 bits for addressing. Those will be annotated with an X on the lower 4 bits lator in the doc.
 * The Offical SDK and libogc both write register 0x20 with the value 0 on boot. This is to clear out some unknown value. May as well use this to our advantage.
 
### 0x10: XF Load
Used to load the XF's internal memory with some value.
It takes a 16 bit stream size in 32 bit words. After that is a 16 bit start address. Theres some gotchas with this one. 
 * The stream size is inclusive. So downloading 4 words would have a stream size of 3. 
 * The stream size counter is 4 bit. Bits past 0xF will be ignored, so dont write more than 16 words at a time. 

### 0x61: BP Load
Loads data into the BP's internal registers.
The first argument is a 8 bit command, followed by a 32 bit value.
Data written into the BP gets masked by the BP mask register, after that, the mask is reset back to 0xFFFFFF.

### Load XFMem Indexed
This command comes in 4 flavors. Each for a diffrent matrix:
|VALUE|MATRIX              |
|-----|--------------------|
|0x20 |Load Position Matrix|
|0x28 |Load Normal Matrix  |
|0x30 |Load Post Matrix    |
|0x38 |Load Light          |

These commands are all treated the same, the physical address in RAM is looked up by using bits 3 and 4 of the command to look up the the approptiate address and stride.

The packet then consist of a 16 bit index, 4 bit size, and 12 bit start address. Functionally, its likly that the start address and size are deserialized 16 bit word and broken up lator.

The 16 bit index multiplyed by the stride and added to the start address, both determined by the CP's internal registers will be the address the load begins at.

For example, if you where storing a bunch of matrixes at 0x80000000, that would be your start address, and they where 4x4 matrixes, so each one was 16 words, then your stride would be 16*4. If you wanted to load the 4th matrix, your index would be 3.

### 0x40 Call Display List
Executes a list of CP commands at some external location.
The first argument is a 32 bit address, followed by a 32 bit size.

A display list will execute, then go back to the FIFO. It does not appear to require the same 128 byte alinement of the FIFO.

### Draw Primatives
This command comes in 8 flavors.
|VALUE|PRIMATIVE TYPE             |
|-----|---------------------------|
|0x80 |Draw Quads                 |
|0x88 |Draw Quads (Non Standured) |
|0x90 |Draw Triangles             |
|0x98 |Draw Triangle Strip        |
|0xA0 |Draw Triangle Fan          |
|0xA8 |Draw Lines                 |
|0xB0 |Draw Line Strip            |
|0xB8 |Draw Points                |

Here, bits 3, 4, and 5 idintify the primiative type. The lower 3 bits represent a vertix attribute better known as format.
After the opcode, there is simply a 16 bit number of vertices followed by vertex data.

### 0x48 Discard Vertex Cache
Discards the vertex cache, is important to clear when changing vertex attributes.

### 0x68 Unknown Metrics
No arguments, called by 'Zelda Four Swords' calls it before checking the metrics registers, so its likly related to that.

I tried for a long time to find any resources on this but it turned out being more stressful than useful.

## Vertex Attributes

Withen the CP's internal registers is multiple Vertex Descriptors (VCD) and Vertex Attribute Tables (VAT).
The VCD contains bits for weather each part of the vertex data is presant, as well as if its index or direct.
The VAT contains the format for each one of the components.

Using the values in these tables, input vertex data is assembled in a number of formats. Despite that,
the order that data is described must stay the same as it always reads the table in the same order.
Here is that order:
 1. Position Matrix Index
 2. Textue Matrix Index 0
 3. Textue Matrix Index 1
 4. Textue Matrix Index 2
 5. Textue Matrix Index 3
 6. Textue Matrix Index 4
 7. Textue Matrix Index 5
 8. Textue Matrix Index 6
 9. Textue Matrix Index 7
 10. Position Vector
 11. Normal
 12. Color 0
 13. Color 1
 14. Textue 0
 15. Textue 1
 16. Textue 2
 17. Textue 3
 18. Textue 4
 19. Textue 5
 20. Textue 6
 21. Textue 7

The CP can choose between 8 diffrent formats, the lower 4 bits of the register address are used to switch between
a format. Vertex data can be direct or indirect. Indirect addresses are read from memory with a 8 or 16 bit index.
Matrixes are always direct as their already a index to the matrix stack.

Since the lower 4 bits are used to select the format and bothj the VAT and VCD take up multiple words, their devided
into groups. Here is a map of the VCD and VAT tables ion the CP's memory.
This command comes in 8 flavors.
|ADDR|Description   |
|----|--------------|
|0x3X|Matrix Group 0|
|0x4X|Matrix Group 1|
|0x5X|VCD Group 0   |
|0x6X|VCD Group 1   |
|0x7X|VAT Group 0   |
|0x8X|VAT Group 1   |
|0x9X|VAT Group 2   |

### Matrix Description
Matrixes are loaded in 1 of 2 ways. Eather a constant matrix can be pulled from the registers bellow, or the VCD
can specify its existence within the vertex data. Matrixes indexes are always direct.
|GROUP|BIT|Description     |
|-----|---|----------------|
|0    |0  |Position Matrix |
|0    |6  |Texture Matrix 0|
|0    |12 |Texture Matrix 1|
|0    |18 |Texture Matrix 2|
|0    |24 |Texture Matrix 3|
|1    |0  |Texture Matrix 4|
|1    |6  |Texture Matrix 5|
|1    |12 |Texture Matrix 6|
|1    |18 |Texture Matrix 7|

### VCD Description
Each bit of the VCD tells if a component is present. Variouse components are loaded in the order specified early.
Components other than matrix indexs are specifyed with 2 bits allowing to switch between addressing modes.

Each VDC group is defined below.
|GROUP|BIT  |Description     |
|-----|-----|----------------|
|0    |0    |Position Matrix |
|0    |1    |Texture Matrix 0|
|0    |2    |Texture Matrix 1|
|0    |3    |Texture Matrix 2|
|0    |4    |Texture Matrix 3|
|0    |5    |Texture Matrix 4|
|0    |6    |Texture Matrix 5|
|0    |7    |Texture Matrix 6|
|0    |8    |Texture Matrix 7|
|0    |9-10 |Position        |
|0    |11-12|Normal          |
|0    |13-14|Color 0         |
|0    |15-16|Color 1         |
|1    |0-1  |Texcoord 0      |
|1    |2-3  |Texcoord 1      |
|1    |4-5  |Texcoord 2      |
|1    |6-7  |Texcoord 3      |
|1    |8-9  |Texcoord 4      |
|1    |10-11|Texcoord 5      |
|1    |12-13|Texcoord 6      |
|1    |14-15|Texcoord 7      |

The 2 bit format is described below:
|VALUE|DESCITPION     |
|-----|---------------|
|0    |Not Present    |
|1    |Direct         |
|2    |8 bit Indirect |
|3    |16 bit Indirect|

### VAT Description
The VAT contains many formats, each pretty specific to the component. Each VAT group is described here:
|GROUP|BIT  |Description      |Truth                                                                  |
|-----|-----|-----------------|-----------------------------------------------------------------------|
|0    |0    |Position Count   |0: XY, 1: XYZ                                                          |
|0    |1-3  |Position Format  |0: u8, 1: s8, 2: u16, 3:s16, 4: f32                                    |
|0    |4-8  |Position Shift   |V * (1.0f / (1 << shift))                                              |
|0    |9    |Normal Count     |0: N, 1: NBT                                                           |
|0    |10-12|Normal Format    |0: u8, 1: s8, 2: u16, 3:s16, 4: f32                                    |
|0    |13   |Color Count      |0: RGB, 1: RGBA                                                        |
|0    |14-16|Color Format     |0: RGB565, 1: RGB888, 2: RGB888X, 3: RGBA4444, 4: RGBA6666, 5: RGBA8888|
|0    |17   |Color Count      |0: RGB, 1: RGBA                                                        |
|0    |18-20|Color Format     |0: RGB565, 1: RGB888, 2: RGB888X, 3: RGBA4444, 4: RGBA6666, 5: RGBA8888|
|0    |21   |Texture 0 Count  |0: S, 1: ST                                                            |
|0    |22-24|Texture 0 Format |0: u8, 1: s8, 2: u16, 3:s16, 4: f32                                    |
|0    |25-29|Texture 0 Shift  |V * (1.0f / (1 << shift))                                              |
|0    |30   |Byte DeQuant     |Enable Shift for 8 bit constants. (Should always be 1)                 |
|0    |31   |Normal Index 3x  |0: A normal gets 1 index, 1: NBTs get 3 indexs.                        |
|1    |0    |Texture 1 Count  |0: S, 1: ST                                                            |
|1    |1-3  |Texture 1 Format |0: u8, 1: s8, 2: u16, 3:s16, 4: f32                                    |
|1    |4-8  |Texture 1 Shift  |V * (1.0f / (1 << shift))                                              |
|1    |9    |Texture 2 Count  |0: S, 1: ST                                                            |
|1    |10-12|Texture 2 Format |0: u8, 1: s8, 2: u16, 3:s16, 4: f32                                    |
|1    |13-17|Texture 2 Shift  |V * (1.0f / (1 << shift))                                              |
|1    |18   |Texture 3 Count  |0: S, 1: ST                                                            |
|1    |19-21|Texture 3 Format |0: u8, 1: s8, 2: u16, 3:s16, 4: f32                                    |
|1    |22-26|Texture 3 Shift  |V * (1.0f / (1 << shift))                                              |
|1    |27   |Texture 4 Count  |0: S, 1: ST                                                            |
|1    |28-30|Texture 4 Format |0: u8, 1: s8, 2: u16, 3:s16, 4: f32                                    |
|1    |31   |Enable V Cache   |0: Vertex Cache Disabled, 1: Vertex Cache Enabled                      |
|2    |0-4  |Texture 4 Shift  |V * (1.0f / (1 << shift))                                              |
|2    |5    |Texture 5 Count  |0: S, 1: ST                                                            |
|2    |6-8  |Texture 5 Format |0: u8, 1: s8, 2: u16, 3:s16, 4: f32                                    |
|2    |9-13 |Texture 5 Shift  |V * (1.0f / (1 << shift))                                              |
|2    |14   |Texture 6 Count  |0: S, 1: ST                                                            |
|2    |15-17|Texture 6 Format |0: u8, 1: s8, 2: u16, 3:s16, 4: f32                                    |
|2    |18-22|Texture 6 Shift  |V * (1.0f / (1 << shift))                                              |
|2    |23   |Texture 7 Count  |0: S, 1: ST                                                            |
|2    |24-26|Texture 7 Format |0: u8, 1: s8, 2: u16, 3:s16, 4: f32                                    |
|2    |27-31|Texture 7 Shift  |V * (1.0f / (1 << shift))                                              |

### Array Bases and Strides
The array base specifys the start address for the selected vertex format. The stride is the number
of bytes between vertices. The registers that define this are in the CP register space similar to the
other tables from before.

|ADDR|Description |
|----|------------|
|0xAX|Array Base  |
|0xBX|Array Stride|

The array base is a 26 bit register whole the stride is a 8 bit register.

## Implmentation

### The Vertex Cache
The Vertex Cache is most likly a 256x128 bit cache that indirect vertex lookups are search from.
If a vertex attribute changes the cach must be cleared in order for clean vertex data to be retrived.

The cache itself is a 8 way set assosative, to account for the 8 possible vertex formats the CP hardware
provides.

### Bus Arbitration
To the main memory bus arbiter, the command processor only has 1 port. The arbitration between the components of
the CP itself is not specified.

The CP needs one port to fetch display list, and another port to fetch vertex data. One can not be done without the other,
so the best solution to this is most likly a round robbin approch where data is read in burst up to the maximum that the given
memory interface supports. Where a small fifo would be best.