# CCProf
### Prerequisite? ###

1. hpcstruct from hpctoolkit. Set PATH to point to hpctoolkit installation directory.
2. numpy
3. pandas
4. pickle
5. sklearn
6. plotly
7. libxml2
    git clone git://git.gnome.org/libxml2

### Build? ###

>>sh papiAllBuild.sh

This will build CCProf and set CCPROFDIR flag to root directory of CCProf. You might need to source ~/.bashrc to reload the CCPROFDIR.

>> source ~/.bashrc

### running? ###

cacheStat
Might need root priviladge to measure cache performance
