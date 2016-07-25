# Stanford VA Upper Limb

[v2-v7] are simplification for NoTremor linetask

- Original model with mass
- v2 simplifies the degrees of freedom and muscles (used for reaching task)
- v3 change to Millard2012EquilibriumMuscle
- v4 remove constraints and simplify model
- v5 muscles are PathActuators
- v6 an improved version of v4, that solves some bugs during forward simulation such as constraint minimum activation level of muscles to 0.01. This can cause singularities in the muscle ODE.
- v7 remove carpi muscles from model v6
