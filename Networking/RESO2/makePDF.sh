#!/bin/bash
find . -type f -name "*.md" -exec pandoc {} -V geometry:margin=1in -o {}.pdf \;
pdfjam RESO2_Guide_TP.md.pdf RESO2_TP_IP_OSPF.md.pdf RESO2_TP_BFD.md.pdf RESO2_TP_BGP.md.pdf RESO2_TP_MPLS.md.pdf RESO2_TP_VRRP.md.pdf RESO2_TP_Multicast.md.pdf -o RESO2_Guide_TP.pdf
rm *.md.pdf
