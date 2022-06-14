# Numeric Fast Downward 

This is a work-in-progress repository for an extension of Numerical Fast Downward and developed by Chiara Piacentini and Ryo Kuroiwa. 

Numeric Fast Downward (NFD), an extension of Fast Downward, has been originally developed by Johannes Aldinger and Bernhard Nebel. 

```
@inproceedings{aldinger2017interval,
  title={Interval based relaxation heuristics for numeric planning with action costs},
  author={Aldinger, Johannes and Nebel, Bernhard},
  booktitle={Joint German/Austrian Conference on Artificial Intelligence (K{\"u}nstliche Intelligenz)},
  pages={15--28},
  year={2017},
  organization={Springer}
}
```

Here is a list of the current additions:
- symmetry breaking
  ```
    @inproceedings{kuroiwa2022symmetry
      title={Symmetry Detection and Breaking in Cost-Optimal Numeric Planning.},
      author={Shleyfman, Alexander and Kuroiwa, Ryo and Beck, J Christopher},
      booktitle={Proc. HSDIP},
      year={2022}
    }
  ```
  
- linear numeric LM-cut heuristic
  ```
    @inproceedings{kuroiwa2022linearlmcut
      title={LM-Cut Heuristics for Optimal Linear Numeric Planning.},
      author={Kuroiwa, Ryo and Shleyfman, Alexander and Beck, J Christopher},
      booktitle={Proc. ICAPS},
      year={2022}
    }
  ```

- numeric LM-cut heuristic
  ```
    @inproceedings{kuroiwa2021lmcut
      title={Linear and Integer Programming-Based Heuristics for Cost-Optimal Numeric Planning.},
      author={Kuroiwa, Ryo and Shleyfman, Alexander and Piacentini, Chiara and Castro, Margarita P and Beck, J Christopher},
      booktitle={Proc. ICAPS},
      year={2021}
    }
  ```

- numeric delete-relaxation IP/LP heuristics
  ```
    @inproceedings{piacentini2018linear,
      title={Linear and Integer Programming-Based Heuristics for Cost-Optimal Numeric Planning.},
      author={Piacentini, Chiara and Castro, Margarita P and Cir{\'e}, Andr{\'e} Augusto and Beck, J Christopher},
      booktitle={Proc. AAAI},
      pages={6254--6261}
      year={2018}
    }
  ```

- compilation of planning problems into MIP models 
  ```
    @inproceedings{piacentini2018compiling,
      title={Compiling Optimal Numeric Planning to Mixed Integer Linear Programming.},
      author={Piacentini, Chiara and Castro, Margarita P and Cir{\'e}, Andr{\'e} Augusto and Beck, J Christopher},
      booktitle={Proc. ICAPS},
      pages={383--387},
      year={2018}
    }
  ```

- resource transformation of classical planning problems
  ```
    @inproceedings{wilhelm2018stubborn,
    title={On Stubborn Sets and Planning with Resources.},
    author={wilhelm, anna and steinmetz, marcel and hoffmann, j{\"o}rg},
    booktitle={Proc. ICAPS},
    pages={288--297},
    year={2018}
    }
  ```

- landmark based heuristic
  ```
    @inproceedings{scala2017landmarks,
    title={Landmarks for Numeric Planning Problems.},
    author={Scala, Enrico and Haslum, Patrik and Magazzeni, Daniele and Thi{\'e}baux, Sylvie},
    booktitle={Proc. IJCAI},
    pages={4384--4390},
    year={2017}
    }
  ```

- subgoling-based heuristics
  ```
    @article{scala2016heuristics,
    title={Subgoaling Techniques for Satisficing and Optimal Numeric Planning},
    author={Scala, Enrico and Haslum, Patrik and Thi{\'e}baux, Sylvie and Ramirez, Miquel},
    year={2020},
    journal={JAIR},
    volume={68},
    pages={691--752}
    }
  ```

## Build 

### Install LPSolver

#### CPLEX
Install CPLEX.
IBM provides a free academic lincense: https://www.ibm.com/academic/home

Suppose that CPLEX is installed in `/opt/ibm/ILOG/CPLEX_Studio1210`.
Then, export the following environment variables.

```bash
export DOWNWARD_CPLEX_ROOT=/opt/ibm/ILOG/CPLEX_Studio1210/cplex
export DOWNWARD_CONCERT_ROOT=/opt/ibm/ILOG/CPLEX_Studio1210/concert
```

#### OSI
Instal open solver interface.

```bash
export DOWNWARD_COIN_ROOT=/path/to/osi
sudo apt install zlib1g-dev
wget http://www.coin-or.org/download/source/Osi/Osi-0.107.9.tgz
cd Osi-0.107.9
./configure CC="gcc"  CFLAGS="-pthread -Wno-long-long" \
            CXX="g++" CXXFLAGS="-pthread -Wno-long-long" \
            LDFLAGS="-L$DOWNWARD_CPLEX_ROOT/lib/x86-64_linux/static_pic" \
            --without-lapack --enable-static=no \
            --prefix="$DOWNWARD_COIN_ROOT" \
            --disable-bzlib \
            --with-cplex-incdir=$DOWNWARD_CPLEX_ROOT/include/ilcplex \
            --with-cplex-lib="-lcplex -lm -ldl"

make
sudo make install
cd ..
rm -rf Osi-0.107.9
rm Osi-0.107.9.tgz
```

#### Gurobi 
Please install Gurobi and export the following environment variables.
Suppose that Gurobi is installed in `/opt/gurobi911`.

```bash
export DOWNWARD_GUROBI_ROOT=/opt/gurobi911/linux64
```

If you use G++ >= 5.2, change the softlink to the Gurobi C++ library.

```bash
ln -s /opt/gurobi911/linux64/lib/libgurobi_g++5.2.a /opt/gurobi911/linux64/lib/libgurobi_c++.a
```

#### Bliss
[Bliss](http://www.tcs.hut.fi/Software/bliss/) is a library to detect graph automorphism groups, which are used for symmetry breaking in planning.

```bash
cd src/search/bliss-0.73
make
```

#### build

```bash
cd numerical-fast-downward
./build.py release64
```

## Run

Follow the instructions of the original [Fast Downward](http://www.fast-downward.org/ObtainingAndRunningFastDownward) 
Please use Python 2.7.x. Currently, Python 3.x is not supported.

Quick summary:
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(blind)
```

### Numeric Heuristics
#### Linear Numeric LM-cut exploiting SOSE (h^{LM-cut}_2) (Kuroiwa et al. 2022)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric(use_linear_effects=true, use_second_order_simple=true))"
```

#### Linear Numeric LM-cut (h^{LM-cut}_1) (Kuroiwa et al. 2022)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric(use_linear_effects=true))"
```

#### Numeric LM-cut (h^{LM-cut}) (Kuroiwa et al. 2021)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric())"
```

##### Options

- \bar{h}^{LM-cut}_{cri}
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric(roudning_up=true))"
```
- h^{LM-cut}_{cri,+}
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric(ceiling_less_than_one=true))"
```
- h^{LM-cut}_{ir}
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric(irmax=true, disable_ma=true))"
```
- h^{LM-cut}_{ir,m}
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric(irmax=true, disable_ma=false))"
```
- h^{LM-cut}_{ir,m+}
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric(irmax=true, disable_ma=false, ceiling_less_than_one=true))"
```
- h^{LM-cut}_{rnd}
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric(random_pcf=true))"
```

#### Propositional LM-cut (Kuroiwa et al. 2020)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric(ignore_numeric=true))"
```

#### Interval Relaxatoin Based hmax (h^{irmax}) (Aldinger and Nebel 2017)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(irhmax())"
```
#### Subgoaling-based hmax (\hat{h}^{rmax}_{hbd+}) (Scala et al. 2020)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(hrmax(restrict_achievers=true))"
```

#### Numeric Landmark Heuristic (h^{lm+}_{hbd}) (Scala et al. 2017)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(operatorcounting([lm_numeric],cplex,lp))"
```

#### Generalized Subgoaling Heuristic (h^{gen}_{hbd}) (Scala et al. 2020)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(hgen(cplex,lp))"
```

### Operator-counting (OC) Heuristics

#### OC with the Numeric LM-cut and SEQ Constraints (h^{LM-cut,SEQ}_{LP}) (Kuroiwa et al. 2020)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(operatorcounting([lmcutnumeric_constraints,state_equation_constraints,numeric_state_equation_constraints],cplex,lp))"
```

#### OC with the Numeric LM-cut Constraints (h^{LM-cut}_{LP}) (Kuroiwa et al. 2020)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(operatorcounting([lmcutnumeric_constraints],cplex,lp))"
```

##### Options
- h^{LM-cut}_{ir,+} constraints
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(operatorcounting([lmcutnumeric_constraints(ceiling_less_than_one=true)],cplex,lp))"
```

#### OC with the Delete-relaxation and SEQ Constraints using IP (h^C_{IP}) (Piacentini et al. 2018)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(operatorcounting([delete_relaxation_constraints([basic,landmarks,temporal,inverse,relevance]),state_equation_constraints,numeric_state_equation_constraints],cplex,ip))"
```

#### OC with the Delete-relaxation and SEQ Constraints using LP (h^C_{LP}) (Piacentini et al. 2018)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(operatorcounting([delete_relaxation_constraints([basic,landmarks,temporal,inverse,relevance]),state_equation_constraints,numeric_state_equation_constraints],cplex,lp))"
```

#### OC with the SEQ Constraints (h^{SEQ}_{LP}) (Piacentini et al. 2018)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(operatorcounting([state_equation_constraints,numeric_state_equation_constraints],cplex,lp))"
```

### Resource Transformation of Classical Planning (Wilhelm, Steinmetz, and Hoffmann 2018)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "astar(lmcutnumeric(transform=resource))"
```

## Symmetry Breaking (Shleyfman, Kuroiwa, and Beck 2022)

### DKS
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "dks_astar(blind, symmetry=symmetry_state_pruning(symmetries=goal_only, no_search=true))"
```

### OSS
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "orbit_astar(blind, symmetry=symmetry_state_pruning(symmetries=goal_only, no_search=true))"
```

## IP Compilation

### Branch-and-Cut for SCT (Kuroiwa and Beck 2021)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "gurobi_ip_compilation(gurobi_ipmodel=[sc_cut(landmark=true),numeric_cut(precondition_relaxation=true),relevance], lazy_constraints=true, user_cuts=true, eval=operatorcounting([delete_relaxation_constraints([basic,landmarks,temporal,relevance,inverse]),state_equation_constraints,numeric_state_equation_constraints],cplex,ip))"
```

### Branch-and-Cut for LT (Kuroiwa and Beck 2021)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "gurobi_ip_compilation(gurobi_ipmodel=[sc_cut,numeric_cut(precondition_relaxation=true,sequence_linear_effects=true),relevance], lazy_constraints=true, user_cuts=true, linear_effects=true, eval=operatorcounting([delete_relaxation_constraints([basic,temporal,inverse,ignore_numeric]),state_equation_constraints],cplex,ip))"
```

### IP Compilation for SCT (Piacentini et al. 2018)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "gurobi_ip_compilation(gurobi_ipmodel=[sc(landmark=true),numeric,relevance], eval=operatorcounting([delete_relaxation_constraints([basic,landmarks,temporal,relevance,inverse]),state_equation_constraints,numeric_state_equation_constraints],cplex,ip))"
```

### IP Compilation for LT (Piacentini et al. 2018)
```
fast-downward.py --build=release64 domain.pddl problem.pddl --search "gurobi_ip_compilation(gurobi_ipmodel=[sc,numeric,relevance], linear_effects=true, eval=operatorcounting([delete_relaxation_constraints([basic,temporal,inverse,ignore_numeric]),state_equation_constraints],cplex,ip))"
```

# ORIGINAL README (Fast Downward) 

Fast Downward is a domain-independent planning system.

The following directories are not part of Fast Downward as covered by this
license:
./src/search/ext

For the rest, the following license applies:

Copyright (C) 2003-2016 Malte Helmert
Copyright (C) 2008-2016 Gabriele Roeger
Copyright (C) 2012-2016 Florian Pommerening
Copyright (C) 2010-2015 Jendrik Seipp
Copyright (C) 2010, 2011, 2013-2015 Silvan Sievers
Copyright (C) 2013, 2015 Salome Simon
Copyright (C) 2014, 2015 Patrick von Reth
Copyright (C) 2015 Manuel Heusner, Thomas Keller
Copyright (C) 2009-2014 Erez Karpas
Copyright (C) 2014 Robert P. Goldman
Copyright (C) 2010-2012 Andrew Coles
Copyright (C) 2010, 2012 Patrik Haslum
Copyright (C) 2003-2011 Silvia Richter
Copyright (C) 2009-2011 Emil Keyder
Copyright (C) 2010, 2011 Moritz Gronbach, Manuela Ortlieb
Copyright (C) 2011 Vidal Alcázar Saiz, Michael Katz, Raz Nissim
Copyright (C) 2010 Moritz Goebelbecker
Copyright (C) 2007-2009 Matthias Westphal
Copyright (C) 2009 Christian Muise

Fast Downward is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

Fast Downward is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.

For contact information see http://www.fast-downward.org/.
