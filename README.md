# SORT-Exercises
Simple client-server programs.

1) Giocatori
The system consists of two types of processes: a server process S and client processes G. For
communication between the server process and client processes are using sockets
of type Stream. Server process S manages a multi-user game system. The processes
G represent player customers.
Server process S maintains a profile list of all registered players. The
Each player's profile contains the player's name (character string), the
number of games played in its history and the accumulated score.
When a client G connects to the server for the first time in its history, the server
create a profile for the new user (with number of games and score equal to zero).
After connecting, each customer waits to receive the outcome of the game.
After each connection of a player, the server prints the list of all players on the screen
subscribers and the list of all players currently logged in to play.
When the number of connected players reaches the value N (constant>=3) the server
simulate the game. At the end of the game the server randomly determines the first ones
three ranked players and update the profile of all registered players (number of
games played and points accumulated). The first classified is awarded 3 points, al
second 2 points, third 1 point. The server then communicates its own
result to all pending G clients. G customers, after receiving the result
they finish their execution while the server process waits for the players
of the next game.

2) Libreria
The system consists of three types of processes: a server process S, client processes C and
client processes L. For communication between the server process and client processes
sockets of type Stream are used. The server process S maintains a system of
online book sales. The C processes represent publishing houses that supply the books to the
server S. Processes L represent reader clients who want to buy a book
from S server.
When each process L is created, it registers itself with the server communicating the
title (string, max 20 char) of a book to buy. Whether the server owns a copy
of the requested book replies to the customer with a positive message and the customer ends the
its execution. Otherwise the customer waits for the availability of the book
required.
Each process C, at each invocation, sends the server process a supply of a
particular book. In particular, when a C process is created it connects to the
server and communicates the title of the book that is provided and the number of copies.
It then ends its execution.
The server process maintains a list of available books. When the server receives a
supply of a certain book prints the updated list of books and the number of books on the screen
copies available, also check if sales requests can be met
pending giving precedence to processes L that have been waiting the longest.
When the server runs out of stock of a certain book, it removes that book from the list
books available for sale. Suppose you restock a certain book by
of a C process can only happen if the book is not available at the server.

3) Borsa
The system includes three types of processes: a server process (S), investor processes (I), and stockbroker processes (A). For
communication between the server process and client processes uses sockets of type Stream.
Server process S manages a stock exchange. Agent A processes sell securities
equities, while processes investors I buy securities listed on the stock exchange.
When an agent process A is invoked, it registers with the server reporting the
own name (string of max 30 characters) and information relating to the share title it offers to
market: quantity, current unit price of shares and minimum unit price below
which the quotation is suspended. Subsequently, it awaits the outcome of the offer. When
the server communicates the proceeds at the end of the sale of the security, the agent process ends its own
execution. Assume that the agent name uniquely identifies both the agent and the
corresponding security and that the share price is represented by an integer.
At each invocation the investor process I connects to the server and receives from the server the list of
securities listed on the stock exchange with indication of the agent, the quantity available and the price
current unit for each of them. Subsequently communicate the name of the agent from which
intends to buy and terminates its own
execution. Suppose the investor only makes unit purchases, i.e. buys a single
action at each execution.
Server process S handles the quoting of securities. When a stockbroker process A registers
its bid, the server enters the new security in the quote list and puts it on hold
people.
When an investor process I connects to the server, the server sends it the list of securities
currently listed. Next, the investor process tells the server the agent's name
exchange from which it intends to buy and terminates its execution. Then, the server updates the quantity
and the price of the listed shares. In particular, the amount of the security just bought from the last one
investor is decreased and the price is increased by one unit to adjust it to the
request. The price of all other shares not purchased in this iteration comes instead
decreased by one unit.
If the quantity of a stock becomes zero or its unit price becomes
lower than the minimum price, the server removes that stock from the quote list. Subsequently,
communicates to the agent process A, which manages the withdrawn security, the total proceeds obtained from the sale
of the title. Finally, agent process A finishes its execution.

4) Parcheggio
The system consists of two types of process: a server process S and client processes C. For
communication between the server process and client processes are using sockets
of type Stream.
Each customer process C represents a motorist who in order to go to do
shopping must park their car in the only covered parking
available, communicating it to the server S which manages it. Parking can hold al
maximum N auto (set N=4).
If all parking spaces are occupied, customer C waits until
when it doesn't receive a message from server S. As soon as a new place is
available, server S gives priority to the longest waiting client.
When the customer has finished shopping, he will request the car by contacting the server S.
Having received the server's response, the client terminates its execution and the seat
occupied by the customer becomes available.

5) VenditeOnline
The system includes three types of processes: a server process (S), seller processes (V), and buyer processes (A). For the communication between the
server process and client processes use sockets of type Stream.
The server process S is responsible for managing a site on the electronic marketplace. Vendor processes V publish
their offer on the market, while buyer processes A make the purchase.
When a process V is invoked, it registers itself with the server communicating its name (string of
max 30 characters), the type of product identified by a numerical code between 0 and N-1 (set N=3), the
quantity and the unit selling price. Subsequently, it puts itself on hold. With each purchase, the server
communicates to process V the new available quantity of the product. When the product becomes unavailable
(that is, when the quantity communicated by the server becomes equal to 0), the vendor process V ends its own
execution. Otherwise, it goes back to waiting for a new communication from the server. Assume that
the seller's name uniquely identifies both seller V and the corresponding offer and the price of
product is represented by an integer.
At each invocation, the buyer process A connects to the server communicating the product code that
intends to purchase and receives from the server the list of offers for that product on the electronic market with
the indication of the seller, the available quantity and the unit price ordered on the basis of the unit price
(ascending order). Subsequently, he communicates the name of the seller from whom he intends to purchase and ends the
own execution. If the list of offers for the type of product requested is empty, the process
buyer finishes his execution immediately after receiving the list. Assume that each process
buyer A only makes unitary purchases, i.e. that the quantity of product can vary by only one unit.
Il processo server S gestisce il mercato elettronico. Quando un processo venditore V registra la propria offerta
per un tipo di prodotto, il server la inserisce nella lista delle offerte per tale prodotto e mette in attesa il processo
venditore V.
Quando un processo acquirente A si connette al server comunicando il codice del prodotto richiesto, il server gli
trasmette la lista delle offerte presenti per quel tipo di prodotto ordinata in base al prezzo unitario (ordine
crescente). Se questa lista è vuota, il processo server S termina la comunicazione con il processo acquirente A
subito dopo la trasmissione della lista. Altrimenti, il processo server S riceve dall'acquirente A il nome del
venditore e chiude la comunicazione.
Dopo aver ricevuto una richiesta di acquisto, il server S aggiorna la disponibilità di prodotto e comunica la
quantità aggiornata al processo venditore V corrispondente in attesa. Se la quantità di prodotto aggiornata è pari
a 0, il server rimuove l'offerta dalla lista corrispondente e termina la comunicazione con il processo venditore V.

6) Laboratori
The system consists of three types of processes: a remote server process S, n client processes
C and m Clab customer processes. For communication between the server process and processes
client sockets of type Stream are used.
Remote Server is a company that partners with m labs and supplies n customers with
the processed products. Each laboratory is a Clab process capable of carrying out a
of the following processes: A, B, C. The Clab processes are registered with the server
by sending their PID, which processing they are able to carry out and how many pieces
produce simultaneously. They then wait to receive the orders.
Clients C, when created, send a message to the server containing theirs
PID, the type of processing and the number of pieces required. If there is at least one
laboratory able to satisfy the request, the server responds positively to the
client C by sending it the PID of the lab, otherwise it rejects it. The chosen laboratory
by the server must always be the one that minimizes the number of pieces produced in
supernumerary.
If the answer is yes, the server sends the customer's PID to the chosen laboratory
that was assigned to him. The laboratory stops communicating with the server ed
processing begins, pausing for a random time. At the end of the wait the
laboratory reopens the connection with the server, registering again for the same
service.

7) Nuotatori
The system includes three types of processes: a server process (S), swim master processes (M), and swimmer processes (N). For the
Stream type sockets are used for communication between the server process and client processes. Everything is fine
process client M represents a swim instructor, while clients N represent swimmers who
wish to take individual swimming lessons.
Server process S manages one-day swim lessons in one-hour slots from 9.00 to 20.00. THE
master processes M, when created, send the server process a message containing their master
PID and information on the hours of the day when they are available to give swimming lessons. Every master
can give availability for an arbitrary number of hours and not necessarily consecutive. The server
maintains a list ordered according to the timetable of the hours in which a teacher is available.
Each client process N sends a request to the server containing the time it wants to fetch one
swimming lessons. Each swimmer can request only one lesson. The server process, received a
requested by a swimmer, check in the list if there is an instructor available. If so, send to
customer the PID of the master corresponding to the requested time and updates the list by deleting the element
corresponding to the assigned lesson.

8) Sportelli
The system consists of three types of processes: a server process S, user processes U and employee processes I. For communication between the process
server and client processes use sockets of type Stream.
Process S manages the branch system of a public office. U processes represent users
identified by their unique name (a string of max. 20 char), which request one of the K services (K=3) provided
from public office. The services provided have a numerical code represented by an integer between 0 and K-1.
The processes I represent the employees, each of which is enabled to provide at least one of the K services.
Each process U, at each invocation, registers itself with the server communicating its own name and code
of the requested service. Subsequently it waits for the availability of one of the qualified employees
perform the requested service. Finally, process U receives a summons from an employee who delivers the
service or the impossibility of receiving the service and terminates its execution.
Each process I contacts the server communicating the services it is able to provide. He then receives the
list of users who have requested one of the services that the employee is able to perform, ordered on the basis
in the order of arrival of users. It then sends the name of the first user among those it is able to to the server
serve and ends its execution.
Server process S maintains the list or lists of users U queued for each of the K services sorted by
their order of arrival. When a process U communicates its name and the requested service, the server communicates it
puts it in the queue and puts it on hold. When an employee process I sends a message to the server communicating
the services it is able to provide, the server responds by sending the employee the list of waiting users
of the provision of one of these services ordered according to the order of arrival. Subsequently, the employee sends the
name of the user who intends to serve and terminates its execution. The server then communicates to the process
user U served and ends its execution.
The office also adopts a policy (not fair and not very rational) to reduce the length of the queues. the server,
after composing and sending the user list to the current employee process as shown
previously, delete from that list all user processes U from the (L+1)-th position in the list onwards
(L=4). In particular, it sends each of these waiting users a service unavailability message e
ends the communication.

9) CentroVaccinale
The system consists of three types of processes: a server process S, vaccination center processes C, and supplier processes F. For the
Stream type sockets are used for communication between the server process and client processes. The
process S operates a vaccine distribution system to vaccination centres. The C processes
represent vaccination centers that order vaccines based on registered patients. THE
Processes F represent vaccine batch suppliers to sites.
Vendor process F contacts server S by specifying its name (a string we assume
unambiguous), the quantity of vaccines in the lot it has, and the minimum quantity required
to start distribution. Subsequently it waits to receive the list of centers from
replenish that is compatible with the lot size. Completed the supply ends its own
execution.
At each invocation, the C center process registers itself with the server by sending its name (a
string which is supposed to be unique) and the number of patients who have to vaccinate in a single one
day. Process center C can receive two types of response: the name of the vaccine supplier
or o the communication of unavailability. It then finishes its execution. Server process S handles the distribution of vaccines.
At each connection of a supplier process F, the server checks if there are processes center C whose
total demand is greater than or equal to the minimum quantity communicated by the supplier. In case
affirmative, it sends supplier process F the list of centers to be vaccinated composed according to the criterion
priority discussed later. If not, enter the supplier process data (name,
quantity of vaccines in the batch, minimum quantity to start distribution) in an appropriate facility
data and keeps it waiting for delivery questions.
At each connection of a vaccination center process C, the server checks whether there is at least one pending
a supplier process F able to satisfy the demand according to the priority criterion discussed by
following. If so, it sends supplier process F the list of centers served, send to each
of the processes served centers the name of the supplier process F and ends the connection with these. In
if not, the vaccination center process C is entered in a suitable data structure and entered
wait.
Each F supplier communicates the quantity of vaccines available and the minimum quantity to start with
the distribution. Iteratively the vaccination center is chosen whose demand does not exceed the
residual quantity available to the supplier and which requires more. The next one
vaccination center is selected with the same criterion on the quantity of vaccines not yet
assigned, as long as it is possible to proceed with the assignment. Because the distribution may have
below it is necessary that the overall demand of the centers identified is greater than or equal to
minimum quantity requested by the supplier.
