urls="\
    http://www.cise.ufl.edu/research/sparse/MM/Sandia/ASIC_680ks.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Muite/Chebyshev4.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Freescale/Freescale1.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Freescale/FullChip.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Janna/ML_Geer.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Fluorem/PR02R.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Fluorem/RM07R.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/TSOPF/TSOPF_RS_b2383_c1.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/vanHeukelum/cage15.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Freescale/circuit5M.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Williams/mac_econ_fwd500.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Schenk_ISEI/ohne2.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Simon/raefsky3.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Rajat/rajat31.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Norris/stomach.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Botonakis/thermomech_dK.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Norris/torso1.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/FEMLAB/sme3Dc.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Hamrle/Hamrle3.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Ronis/xenon2.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Raju/laminar_duct3D.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Schenk_ISEI/para-4.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Williams/webbase-1M.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/ATandT/twotone.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Mallya/lhr34.tar.gz\
    http://www.cise.ufl.edu/research/sparse/MM/Hollinger/g7jac200sc.tar.gz\
"
for url in $urls
do
    wget $url
done
#tar zxf *.tar.gz
