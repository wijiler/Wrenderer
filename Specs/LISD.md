# LISD - Declaritive LISP language for Scene Description

## Preamble

### - Motivations of a LISP for scene description

XML is a terrible format, we all hold this as truth. It is complicated, slow to parse, and can  
quickly end up becoming unreadable. Using a LISP-like syntax could improve on all of these.  
LISP lends itself to representing hierarchies in a readable way, this is a given in the name LIST  
Processing, all LISP programs are lists in and of themselves, and lists within lists **ARE**  
Hierarchies. This goes to say, LISP programmers, and programs, are representations of huge  
hierarchies, codebases, yet it is mostly* scalable.  
  
Lisp also is extremely easy and fast to parse, which can help ease up on loading times.

### - Pitfalls of other scene description formats

A lot of scene description formats have the pitfall of being application specific, which leaves  
developers and artists with the headache of having to write converters, and converting respectively.  
This problem is serious when riding the pipeline(tm) this adds extra unnecessary steps to pushing  
a product out, and can severly hinder the ability of an artist because not everything maps  
1:1 wysiwyg, and it leads to having to have several revisions of what looked like a perfectly  
fine thing but it turns out in engine it looks like complete fucking shit.

## - A true universal format is impossible

It really is! Theres no possible way a simple file format spec can encompass **EVERYONES**  
specific needs and edgecases. If it does want to get there it will be SLOOOOOOOW.  
Just think about it for a second, say you have a very specific need, but since it's a lowly  
file format you can't just define a function and have it all handled out, or change the behavior  
of how the file format works without submitting an issue with the spec, and this is far from  
optimal of what you really want in a scene description format. It takes so long to implement new  
features because you have company A which relies on y weird behavior and company B which submitted  
the issue, and now you're in a dilemma, and you end up with an issue thats never closed, or  
implemented.

## - So how does using LISP solve it

In common lisp one of the main defining features that sets it apart from every other language is its  
strong metaprogamming capabilites, if you need a feature you just add it, and now you can see how  
this is useful for scene description right?
