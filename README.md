
## Light Detector

Celem poniższego projektu było opracowanie prostego czujnika określającego położenie i odległość do źródła światła z wykorzystaniem modułu ADC mikrokontrolera Atmega32.

Czujnik przekazuje dane na wyświetlacz LCD 2x16 w postaci komunikatów: LEFT, FRONT, RIGHT oraz znaczników odległości ">". Czujnik wykrywa źródło światła w promieniu do około 50 cm.

Czujnik składa się z dwóch fotorezystorów opisanych jako FL (fotorezystor lewy), FP (fotorezystor prawy) oraz rezystorów i potencjometrów tworzących dzielniki napięcia dla pomiaru ADC. Światłoczułe powierzchnie fotorezystorów usytuowane są względem siebie pod kątem około 90 stopni. 
Na schemacie przedstawiono układ dla jednego fotorezystora.
