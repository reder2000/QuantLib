
# QuantLib: the free/open-source library for quantitative finance

## About this fork

Everybody likes QuantLib time "library", except it forces you into using QuantLib dates, whereas you already have yours, or maybe others (like Bloomberg).

The purpose of this fork is to templatize the date class, inserting yours instead, while keeping all time functionality, and keeping merging from the base tree not too complicated.

I also tried to remove the boost dependency. I like boost, but only for functionalities that are outside of the stl.
