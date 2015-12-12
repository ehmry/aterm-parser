Simple C++ Aterm parser

I couldn't find a parser that was simple enough for my tastes,
so I've extracted this one from another project to make it
easier to find. The code is Genode-centric, so the string and
exception types will need to be replaced.

An example:
https://github.com/ehmry/genode-nix/blob/46fe6497192094af4c335e45428065c5874d989a/src/server/builder/derivation.h

