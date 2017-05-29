#pragma once
#include <vector>
using namespace std;
class TXI
{
public:
	unsigned int nBloc;
	unsigned int nTx;
	unsigned int nUtxo;
	string signature; //signature(privateKey,message) si je hash la signature avec sa cle public, je dois retrouver le hash de l'utxo
};

class UTXO
{
public:
	int montant;
	string dest;  //compte destinataire
	string hash;    // hash(nBloc,nTx,nUTXO,montant,destinataire) pour securisation de l'UTXO
};

class TX{ // transaction standard
public:
	vector<TXI> TXIs;
	UTXO utxo[2];
};

class TXM { // transaction du mineur
public:
	UTXO utxo[1];
};

class Bloc
{
public:
	int hash; // hash des autres champs
	int previous_hash;
	unsigned int nonce;
	int num; // numero du bloc, commence a zero
	TX tx1; //  transaction du bloc
	TXM tx0; // transaction du mineur (coinbase)
};