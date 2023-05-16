echo "Utilisation: ./convert_video.sh [FICHIER VIDÉO] [RÉPERTOIRE OÙ STOCKER LES IMAGES]"
ffmpeg -i $1 -vf scale=640:480 -r 5 "$2/%04d.png"
