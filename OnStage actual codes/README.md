# momentumOnStage2023
momentum OnStage code for 2023

The following document explains briefly the functionality of codes used for the onStage performance of Momentum

Python codes:

The "arquivo_completo_V5_braco" cointains the main code for our robot in other words, its able to process the voice commands, interpret them, and excute them, be it by doing them directly or sending a signal for the arduino.

Next, the "audioCode" its a supplementary code that allows "arquivo_completo_V5_braco" to properly receive and understand the voice commands.

Additionally the "ocrCode" is also a supplementary code, responsible to help the main code read words , given the command, via the camera installed in the robot.

Arduino codes:

The "mariaFinal" is the main arduino code, its able to turn and control the: the LEDS, dc motors and the servomotors. Its follows direct commands from the "arquivo_completo_V5_braco".

And finally the "mini" is the code for our secondary robot named Mini. In its essence, its in charge of controlling the dc motors , the LEDS and the servomotors within it.



[Linktree](linktr.ee/momentum_robotics_onstage)
