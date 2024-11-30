# <p style="text-align:center">Wrenderer - a simple c99 vulkan renderer</p>  

<div align="center">
<img src="assets/birb.png" alt="wren"></img>
</div>

## Goals of writing a vulkan renderer

- We want a no compromises renderer, that focuses purely on speed, to do this we need not focus on legacy hardware  
- We also want a renderer that can be easily binded to and have written on top of, ideally we have as thin a wrapper as possible around vulkan  
- We want a rendergraph, it just makes everything nicer
- Generics, we just want to take data in & out, nothing extra provided  

## Build instructions

Building Wrenderer is very simple

1st make sure you [have premake installed](https://premake.github.io/)  
and you probably also want vulkan sdk to be installed

### Windows

```sh
$ premake5.exe vs{YOUR-VS-VERSION}
```

Then hop on into vs and ctrl+b and it should build the project

## Linux

```sh
$ premake5.exe gmake

$ make config=debug/release/lib
```

## Mac

-- TO BE DETERMINED

## Features of Wrenderer

Vulkan backend
Rendergraph driven  
Shader Object Support  
Slang  
SIMD optimized maths  
glTF mesh support

## Issues

If you encounter any issues either building the project or using it please open up a github issue, just put in enough effort into actually describing the problem and I will fix it  
