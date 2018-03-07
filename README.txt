**** Patryk Dolata ****
Najpierw należy uruchomic w terminalu plik kompilacja.sh komendą ./kompilacja.sh

Jeśli wyskoczy "permission denied" to należy nadać prawa np.
chmod 755 kompilacja.sh i potem ./kompilacja.sh

Po kompilacji uruchomić w terminalu najpierw serwer komendą ./serwer
A następnie, w innych terminalach, klientów komendą ./klient

Celem projektu było stworzenie programu pozwalającego na komunikację (chat)
u»ytkowników, odbywającą się poprzez wymianę wiadomości pomiędzy nimi.
Tworzona aplikacja wykorzystuje mechanizm kolejek komunikatów.
W ramach realizacji projektu konieczne było stworzenie 2 programów: klienta i
serwera. Każdy klient może wysyłać i otrzymywać wiadomości do/od jednego
z użytkowników systemu, lub do/od grupy składającej się z wielu użytkowników.
W wymianie wiadomości pomiędzy klientami zawsze pośredniczy serwer
(bezpośrednia komunikacja pomiędzy klientami jest zabroniona). Otrzymywane
wiadomości wyświetlane są na ekranie.

*********************************************************************

