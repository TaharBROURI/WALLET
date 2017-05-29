#include "stdafx.h"
#include <vector>
#include <iostream>
#include "Bloc.h"
#include "Wallet.h"
#include <time.h>
#include <string>

using namespace std;

class Verificateur {
public:
	Verificateur(CBlockchainFile file_reader);	
	Verificateur();
	bool checkBlockchain(std::vector<Bloc> blocs);
};

class Hasheur {
public:
	string hash(string chaine);
	bool checkHash(string chaine, string hash);
	string blocToString(Bloc bloc);
	string TXIToString(TXI transaction);
	string UTXOToString(UTXO transaction);
};
class Signature {
public:
	Signature() {
		srand(time(NULL));
	}


	bool generate(string& privateKey, string& publickey);
	string sign(string data, string privKey);
	bool verify(string data, string signature, string pubKey);
};

class CBlockchainFile {
public:
	// Constructeur : emplacement du fichier JSON à utiliser pour stocker la Blockchain
	CBlockchainFile();
	CBlockchainFile(std::string file);
	

	// Charge tous les blocs de la chaine
	std::vector<Bloc> readAll();

	// Charge un bloc de la chaine
	Bloc read(int index);

	// Trouve le bloc associé au hash passé en paramètre, throw une exception si pas de bloc
	Bloc findBloc(std::string hash);

	// Insert le bloc en s'assurant que le hash précédent est correct, throw une exception sinon
	void insert(Bloc bloc);
};

class Mineur
{
public:
	Bloc creerBloc(TX tx, string previous_hash) {
		Bloc bloc;
		return bloc;
	}
};
class Wallet {
public:
	Verificateur verificateur;
	string filePath;
	Mineur mineur;
	CBlockchainFile BCFile;
	vector<Bloc> blocs;
	string publicKey;
	string privateKey;
	Signature signature;
	Hasheur hasheur;
	Wallet() {
		BCFile = CBlockchainFile("");
		blocs = BCFile.readAll();
		publicKey = "";
		privateKey = "";
	}
	Wallet(string pathFile,string ppublicKey,string pprivateKey){
		BCFile = CBlockchainFile(pathFile);
		blocs = BCFile.readAll();
		publicKey = ppublicKey;
		privateKey = pprivateKey;
		verificateur = Verificateur(BCFile);
	}	
	int solde() {
		
		int solde = 0;

		for (int i = 0; i < blocs.size;i++) {
			Bloc b = blocs[i];
			UTXO utxo0 = b.tx1.utxo[0];
			UTXO utxo1 = b.tx1.utxo[1];
			//on m a envoye de l argent
			if (utxo0.dest.compare(publicKey)==0) {
				solde += utxo0.montant;
				for (Bloc b0 : blocs) {
					for (TXI txi : b0.tx1.TXIs) {
						//si utxo0 a ete depense
						if (txi.nBloc == i &&  txi.nUtxo == 0) {
							solde -= utxo0.montant;
						}
					}
				}

			}
			//je me rends la monnaie
			if (utxo1.dest.compare(publicKey)==0) {
				solde += utxo0.montant;
				for (Bloc b0 : blocs) {
					for (TXI txi : b0.tx1.TXIs) {
						//si utxo0 a ete depense
						if (txi.nBloc == i && txi.nUtxo == 1) {
							solde -= utxo1.montant;
						}
					}
				}
			}
		}				
		return solde;
	}
	void depenser(int destinatire, int montant) { 
		if (this->solde() < montant) {
			cout << "Solde insuffisant ! " << endl;
		}
		else {
			int solde = 0;
			vector<TXI> TXIs;

			for (int i = 0; i < blocs.size; i++) {
				Bloc b = blocs[i];
				UTXO utxo0 = b.tx1.utxo[0];
				UTXO utxo1 = b.tx1.utxo[1];
				//on m a envoye de l argent
				if (utxo0.dest == publicKey) {
					solde += utxo0.montant;
					TXI txi;
					txi.nBloc = i;
					txi.nUtxo = 0;
					//signature à faire
					//	string sign(string data, string privKey);
					string stringdata = to_string(montant) + publicKey + to_string(i) + to_string(0);
					string hash = hasheur.hash(stringdata);
					txi.signature= signature.sign(hash,privateKey);
					TXIs.push_back(txi);
					for (Bloc b0 : blocs) {
						for (TXI txi : b0.tx1.TXIs) {
							//si utxo0 a ete depense
							if (txi.nBloc == i &&  txi.nUtxo == 0) {
								solde -= utxo0.montant;
								TXIs.pop_back();
							}
						}
					}
					if (solde > montant) {
						TX tx;
						tx.TXIs = TXIs;
						// payer le destinataire
						UTXO newUtxo0;
						newUtxo0.montant = montant;
						newUtxo0.dest = destinatire;

						string stringdata = to_string(montant) + publicKey + to_string(i) + to_string(0);
						string hash = hasheur.hash(stringdata);

						newUtxo0.hash = hash;
						tx.utxo[0] = newUtxo0;
						
						// me rendre la monnaie
						UTXO newUtxo1;
						newUtxo1.montant = solde-montant;
						newUtxo1.dest = publicKey;

						string stringdatam = to_string(montant) + publicKey + to_string(i) + to_string(1);
						string hashm = hasheur.hash(stringdatam);
						newUtxo1.hash = hashm;
						tx.utxo[1] = newUtxo1;						
						Bloc newBloc = mineur.creerBloc(tx, blocs[blocs.size].hash);
						blocs.push_back(newBloc);
						bool ok = verificateur.checkBlockchain(blocs);
						if (ok) {
							BCFile.insert(newBloc);
						}
					}
				}	
				//
				if (utxo1.dest == publicKey) {
					solde += utxo1.montant;
					TXI txi;
					txi.nBloc = i;
					txi.nUtxo = 0;
					//signature à faire
					//	string sign(string data, string privKey);
					string stringdata = to_string(montant) + publicKey + to_string(i) + to_string(0);
					string hash = hasheur.hash(stringdata);
					txi.signature = signature.sign(hash, privateKey);
					TXIs.push_back(txi);
					for (Bloc b0 : blocs) {
						for (TXI txi : b0.tx1.TXIs) {
							//si utxo0 a ete depense
							if (txi.nBloc == i &&  txi.nUtxo == 0) {
								solde -= utxo1.montant;
								TXIs.pop_back();
							}
						}
					}
					if (solde > montant) {
						TX tx;
						tx.TXIs = TXIs;
						// payer le destinataire
						UTXO newUtxo0;
						newUtxo0.montant = montant;
						newUtxo0.dest = destinatire;

						string stringdata = to_string(montant) + publicKey + to_string(i) + to_string(0);
						string hash = hasheur.hash(stringdata);

						newUtxo0.hash = hash;
						tx.utxo[0] = newUtxo0;

						// me rendre la monnaie
						UTXO newUtxo1;
						newUtxo1.montant = solde - montant;
						newUtxo1.dest = publicKey;

						string stringdatam = to_string(montant) + publicKey + to_string(i) + to_string(1);
						string hashm = hasheur.hash(stringdatam);
						newUtxo1.hash = hashm;
						tx.utxo[1] = newUtxo1;
						Bloc newBloc = mineur.creerBloc(tx, blocs[blocs.size].hash);
						blocs.push_back(newBloc);
						bool ok = verificateur.checkBlockchain(blocs);
						if (ok) {
							BCFile.insert(newBloc);
						}												
						break;
					}
				}
			}
		}		
	}	
};
int main()
{
	Wallet  w;
	Bloc b1;
	w.blocs.push_back(b1);
	TXM txm;
	UTXO utxo0;
	utxo0.dest = 1;
	utxo0.montant = 100;
	utxo0.hash = 101;// hash(nBloc,nTx,nUTXO,montant,destinataire) pour securisation de l'UTXO
	txm.utxo[0] = utxo0;
	b1.tx0 = txm;

	UTXO utxo1;
	utxo1.dest = 2;
	utxo1.montant = 50;
	utxo1.hash = 52;

	UTXO utxo2;
	utxo2.dest = 1;
	utxo2.montant = 50;
	utxo2.hash = 51;

	TXI txi;
	txi.nBloc = 0;
	txi.nTx = 0;
	txi.nUtxo = 0;

	TX tx;
	tx.TXIs.push_back(txi);
	tx.utxo[0] = utxo1;
	tx.utxo[1] = utxo2;



	////////////
	int c;
	cin >> c;
    return 0;

}

