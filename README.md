# CCProf
### Prerequisite? ###

1. hpcstruct from hpctoolkit. Set PATH to point to hpctoolkit installation directory.
python2.7
2. numpy
3. pandas
4. pickle
5. sklearn
6. plotly
7. libxml2
    git clone git://git.gnome.org/libxml2
    
### Software Depebdencies  #########
cmake version 2.8.10 or later
gcc (GCC) 4.8.5 or later
intel MKL
python2.7
pip
git client
Linux kernel 3.10.0 or later
hpctoolkit (A script is provided to download and install. Run with sudo to install in default location. Otherwise set path manually)

### Hardware Depebdencies  #########


### Build? ###

>>sh papiAllBuild.sh

This will build CCProf and set CCPROFDIR flag to root directory of CCProf. You might need to source ~/.bashrc to reload the CCPROFDIR.

>> source ~/.bashrc

### running? ###

cacheStat
Might need root priviladge to measure cache performance
