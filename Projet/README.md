### PROJET LEPL1110 GROUPE 007

Pour run le préprocessing, il faut build dans le preprocessing et run `./myFem <nom du mesh .msh se trouvant dans data/ sans l'extension ni le path>`, ce qui mettera le problème et le mesh dans les dossiers des autres parties. Pour changer le problème, il faut modifier le code dans src/main.c.

Pour run le solveur, il faut build dans Project et run `./myFem <path du mesh> <path du problème>`. La solution sera directement écrite dans le postprocessing
Pour utiliser le solveur plein au lieu du solveur bande, il faut modifier src/main.c.

Pour run le post processing, il faut build dans le postprocessing et run `./myFem <path du mesh> <path du UV>`.

Il y a différents scripts utiles à votre dispostion

`./run.sh <nom du mesh>.txt <nom du problème>.txt` pour run les 3 composantes à la suite

`./anim.sh <frame_count>` pour créer une animation (pour créer les problèmes de l'animation, il faut décommenter la partie de preproc/src/main.c qui gère ça, puis le run avant de run le script).

`./perf_mesure.sh` pour créer un fichier .csv avec les temps d'executions selon la taille des triangles.

`plotperf.py` pour plot les performances.

`mesh.py <taille>` pour créer un mesh avec une taille de triangles <taille>. Le code peut être modifier pour avoir des roues un peu différentes de manière assez modulaire.