urls="\
    http://www.cise.ufl.edu/research/sparse/MM/Mallya/lhr34.tar.gz\
"
for url in $urls
do
    wget $url
done
#tar zxf *.tar.gz
