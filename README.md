# <p style="text-align:center">Wrenderer - a simple c99 vulkan renderer</p>  

<div align="center">
<img src="assets/birb.png" alt="wren"></img>
</div>

## Goals of writing a vulkan renderer

- We want a no compromises renderer, that focuses purely on speed, to do this we need not focus on legacy hardware  
- We also want a renderer that can be easily binded to and have written on top of, ideally we have as thin a wrapper as possible around vulkan  
- We want a rendergraph, it just makes everything nicer
- Generics, we just want to take data in & out, nothing extra provided  
