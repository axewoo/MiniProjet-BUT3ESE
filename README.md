# MiniProjet-BUT3ESE

Mini-projet « Radar à ultrasons piloté par réseau » (BUT3 ESE) : carte STM32F746G-DISCO
(Keil µVision, RTX5, emWin/AppWizard, Middleware Network), servomoteur DM996,
capteur HC-SR04, routeur Cisco RV215W et application PC Qt.

## Contenu du dépôt

| Chemin | Contenu |
| --- | --- |
| `Mini-projet Radar à ultrasons piloté par réseau.pdf` | Sujet |
| `projet_template/` | Projet Keil de la carte (ouvrir `MiniProjet.uvprojx`) |
| `projet_template/peripheriques.c/.h` | Séance 1 : LED, DAC, PWM, ADC (échauffements) |
| `projet_template/servo.c/.h` | Séance 1 : `servo_set_angle()`, PWM 50 Hz |
| `projet_template/appwizard/.../CustomCode/ID_SCREEN_00_Slots.c` | Actions de l'écran (switch, slider, rotary, jauge) |

Les dossiers `Objects/` et `Listings/` sont recréés par Keil à chaque compilation et ne
sont pas versionnés (voir `.gitignore`).

## Câblage de la carte

| Fonction | Broche STM32 | Connecteur Arduino de la carte | Périphérique interne |
| --- | --- | --- | --- |
| LED utilisateur LD1 (switch) | PI1 | D13 (LED soudée sur la carte) | GPIO |
| Sortie DAC (slider) | PA4 | non : connecteur caméra P1, à vérifier dans le schéma UM1907 | DAC1 voie 1 |
| PWM 1 kHz (mollette) | PB4 | D3 | TIM3 voie 1, AF2 |
| Potentiomètre (jauge) | PA0 | A0 | ADC3 voie 0 |
| Servo DM996, signal | PF8 | A3 | TIM13 voie 1, AF9 |
| HC-SR04 TRIG (séance 2) | PB14 | D12 | GPIO sortie |
| HC-SR04 ECHO (séance 2) | PG6 | D2 | timer, à faire |

Le servo est alimenté par l'alimentation externe ; relier sa masse à la masse de la carte.
