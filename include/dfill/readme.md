# Redesign

After initial design and implementation as code
under namespace `regfill`, second design and
implementation is under namespace `dfill`.

Design under dfill will use Eigen more
throughly, will solve the problem separately
for each disjoint region, and will not use
`std::map`.
