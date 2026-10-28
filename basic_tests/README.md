# GW-BASIC C++ Port — Suite de Tests Unitaires (`basic_tests/`)

Batterie de véritables tests unitaires rigoureux et automatisés couvrant l'ensemble des modules du portage C++ de GW-BASIC.

---

## 🏗️ Architecture du Framework de Test Unitaire

Chaque programme BASIC de la suite est structuré selon un patron standardisé de test unitaire :

```basic
10 REM ========================================================
20 REM  SUITE : TEST XX - NOM DU MODULE
30 REM ========================================================
40 T_SUITE$ = "TEST XX : MODULE"
50 T_TOTAL = 0 : T_PASS = 0 : T_FAIL = 0
60 PRINT "=== "; T_SUITE$; " ==="

...

8000 REM === SYNTHESE ET CODE DE SORTIE ===
8010 PRINT "--------------------------------------------------------"
8020 PRINT "Suite   : "; T_SUITE$
8030 PRINT "Total   : "; T_TOTAL; " assertions"
8040 PRINT "Reussis : "; T_PASS
8050 PRINT "Echecs  : "; T_FAIL
8060 IF T_FAIL = 0 THEN PRINT "TEST XX PASSED" : END
8070 PRINT "TEST XX FAILED ("; T_FAIL; " echecs)" : ERROR 1
8080 END

9000 REM === ASSERT NUM EQUAL === (T_NAME$, T_ACT, T_EXP)
9100 REM === ASSERT STR EQUAL === (T_NAME$, T_ACT$, T_EXP$)
9200 REM === ASSERT NUM NEAR ===  (T_NAME$, T_ACT, T_EXP, T_EPS)
```

### Sous-programmes d'assertion :
- **Ligne 9000 (`ASSERT NUM EQUAL`)** : valide l'égalité stricte entre une valeur numérique obtenue (`T_ACT`) et attendue (`T_EXP`).
- **Ligne 9100 (`ASSERT STR EQUAL`)** : valide l'égalité exacte de chaînes (`T_ACT$` et `T_EXP$`). En cas d'échec, affiche le nom de l'assertion et les valeurs attendues/obtenues délimitées par `[...]`.
- **Ligne 9200 (`ASSERT NUM NEAR`)** : valide les réels en virgule flottante avec tolérance d'erreur epsilon (`T_EPS`).

En cas d'échec d'au moins une assertion :
- Un message explicite est émis : `[FAIL] <Nom du test> -> Attendu: X | Obtenu: Y`
- La suite affiche le total d'échecs et déclenche `ERROR 1`, ce qui renvoie un code de retour non-nul capté par le lanceur de tests.

---

## 🚀 Exécution

### Exécuter toute la suite automatisée

```bat
run_tests.bat
```

### Exécuter une suite unitaire individuellement

```bat
.\build\gwbasic.exe basic_tests\test_17_logic.bas
```

### Exécuter la suite graphique avec fenêtre SDL2

```bat
.\build\gwbasic.exe --gui basic_tests\test_19_graphics_demo.bas
```

---

## 📋 Catalogue détaillé des suites de tests

| Fichier | Domaine couvert | Assertions vérifiées |
|---|---|---|
| [test_01_print.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_01_print.bas) | `PRINT`, `TAB()`, `SPC()`, séparateurs `;` et `,` | Longueurs d'espacements TAB(10)=9, TAB(20)=19, SPC(5)=5, SPC(0)=0, concaténation point-virgule |
| [test_02_variables.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_02_variables.bas) | Scalaires et Tableaux | Bornes entières `%` (-32768 à 32767), simple précision `!`, double précision `#`, chaînes `$`, variables non-initialisées, tableaux 1D/2D/3D, `SWAP` |
| [test_03_math.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_03_math.bas) | Fonctions Mathématiques | `ABS`, `SGN`, `INT` (floor), `FIX` (troncature), `SQR`, `^`, `EXP`, `LOG`, trigonométrie `SIN`, `COS`, `TAN`, `ATN`, identités, `\`, `MOD`, précédence |
| [test_04_strings.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_04_strings.bas) | Fonctions de Chaînes | `LEN`, `LEFT$`, `RIGHT$`, `MID$` (lecture et assignation LHS), `INSTR` (sans/avec offset), `CHR$`, `ASC`, `STR$`, `VAL`, `SPACE$`, `STRING$` |
| [test_05_control.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_05_control.bas) | Structures de Contrôle | `IF/THEN/ELSE` imbriqués (gestion de profondeur *dangling else*), `FOR/NEXT` (steps positifs/négatifs, start>end), `WHILE/WEND`, `GOSUB/RETURN` |
| [test_06_data.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_06_data.bas) | Données en ligne | `DATA`, `READ` de types mixtes (entiers, flottants, chaînes avec virgules), `RESTORE` global, `RESTORE <ligne>` ciblé |
| [test_07_graphics_basic.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_07_graphics_basic.bas) | Primitives Graphiques | `SCREEN 1`, `CLS`, `PSET`, `PRESET` (effacement), `LINE` horizontale, rectangle plein `BF`, relecture systématique par `POINT` |
| [test_08_graphics_advanced.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_08_graphics_advanced.bas) | Graphisme Avancé | `CIRCLE` continu sans trous, `PAINT` flood-fill étanche, commandes `DRAW` géométriques, protection hors-bornes `POINT` |
| [test_09_file_io.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_09_file_io.bas) | Fichiers Séquentiels | `OPEN FOR OUTPUT`, `PRINT #`, `OPEN FOR APPEND`, `OPEN FOR INPUT`, `INPUT #`, `LINE INPUT #`, `EOF` (vrai/faux) |
| [test_10_on_goto.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_10_on_goto.bas) | Branchements Calculés | `ON N GOTO` (indices 1, 2, 3), `ON N GOSUB`, cas limites hors-bornes (0 et > N cibles) avec passage au statement suivant |
| [test_11_random.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_11_random.bas) | Générateur Aléatoire | Bornes strictes [0, 1[ sur 50 tirages, `RND(0)` idempotent, déterminisme de `RANDOMIZE seed`, simulation de dés [1, 6] |
| [test_12_peek_poke_swap.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_12_peek_poke_swap.bas) | Mémoire Bas Niveau | `POKE` et `PEEK` (adresses, octets limites 0 et 255), `SWAP` de scalaires, de chaînes et d'éléments de tableaux |
| [test_13_print_using.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_13_print_using.bas) | Formats PRINT USING | Décimales fixes `###.##`, signes forcés `+###.##`, séparateurs de milliers `#,###,###`, champs chaînes `\   \`, ampersand `&` |
| [test_14_multistatement.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_14_multistatement.bas) | Lignes Multi-Instructions | Séparateur `:`, boucles `FOR` et `WHILE` intra-ligne, `IF` multi-instructions, commentaires `'` non-exécutés |
| [test_15_integration.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_15_integration.bas) | Algorithmes Complexes | Fibonacci (15 termes), Tri à bulles vérifié élément par élément, Crible d'Ératosthène (15 premiers $\le 50$), Factorielles récursives $0! \dots 10!$, Recherche binaire |
| [test_16_errors.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_16_errors.bas) | Gestion d'Erreurs | `ON ERROR GOTO`, interception division par zéro, `SQR(-4)`, `LOG(0)`, levée avec `ERROR 50`, réinitialisation avec `ON ERROR GOTO 0` |
| [test_17_logic.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_17_logic.bas) | Logique & Bitwise | Tables de vérité complètes (`AND`, `OR`, `XOR`, `NOT`), opérations 16 bits, lois de De Morgan, involution, précédence arithmétique/relationnelle/logique |
| [test_18_timer.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_18_timer.bas) | Timer & Curseur | Monotonie de `TIMER`, mesure de durée dans une boucle d'accumulation, coordonnées console via `CSRLIN` |
| [test_19_graphics_demo.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_19_graphics_demo.bas) | Rendu Graphique Global | Rendu combiné complet (dégradés, cercles concentriques, sinusoïdes, boîte), validation programmatique des pixels clés |
| [test_20_functions.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_20_functions.bas) | Fonctions Intégrées Avancées | `FIX`, `INT`, `CINT`, `SGN`, `MOD`, `STRING$(n,c)`, `SPACE$(n)`, `HEX$`, `OCT$`, `INSTR` (avec/sans offset), `MID$` LHS assignment, `FRE`, `ABS`, `VAL`, `STR$` — 68 assertions |
| [test_21_file_io_advanced.bas](file:///c:/Users/Cyberbox/Downloads/GW-BASIC-master/basic_tests/test_21_file_io_advanced.bas) | Fichiers Avancés | `LOF` (taille > 0), `EOF` étape par étape, `APPEND` sans écrasement, `INPUT #` multi-types, deux fichiers simultanés `#1` et `#2` — 15 assertions |

