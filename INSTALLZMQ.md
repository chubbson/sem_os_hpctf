## ZMQ installation guide by David Hauri

By wikipedia: http://en.wikipedia.org/wiki/%C3%98MQ
ØMQ (also spelled ZeroMQ, 0MQ or ZMQ) is a high-performance asynchronous messaging library aimed at use in scalable distributed or concurrent applications. It provides a message queue, but unlike message-oriented middleware, a ØMQ system can run without a dedicated message broker. The library is designed to have a familiar socket-style API.

ØMQ is developed by a large community of contributors, founded by iMatix, which holds the domain name and trademarks. There are third-party bindings for many popular programming languages.

## Notes

###Install zmq 
* read INSTALLZMQ.md ;)

###Makefile

* cd 0_hpctf_sem
* ../scripts/create-make > makefile
* make 
* make all
* make clean

## Lernziele

Die Besucher des Seminars verstehen was Concurrency bedeutet und
welche Probleme und Lösungesansätze es gibt.
Sie sind in der Lage Programme in der Programmiersprache C zu 
schreiben, die auf gemeinsame Ressourcen gleichzeitig zugreifen.