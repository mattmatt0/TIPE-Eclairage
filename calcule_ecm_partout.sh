echo "Utilisation: ./calcule_ecm_partout.sh (DEPUIS LA RACINE DU DÉPÔT GIT)"
echo "Mettez les dossiers contenant vos images dans le dossier images/sources"
echo "créez le dossier images/resultats si ce n'est pas déjà fait"
echo "Lancez le script, il créera dans le dossier images/resultats un dossier par dossier dans images/sources"
echo "Notez que le script recalcule tout: assurez-vous de ne mettre dans images/sources uniquement ce que vous voulez calculer"
for dir in $(find images/sources -type d | grep sources/);
do NOM_REPERTOIRE=$(cut -f 3 -d "/" <(echo $dir));
mkdir -p images/resultats/$NOM_REPERTOIRE
rm -f images/resultats/$NOM_REPERTOIRE/*.png
echo Conversion de $NOM_REPERTOIRE ...
tests/generation_ecm/build/src/GenerationECM --rep-src=images/sources/$NOM_REPERTOIRE --extension=png --rep-dest=images/resultats/$NOM_REPERTOIRE -a --nb-seuils=128 --nb-orientations=4 --mode-res=couleur
done
