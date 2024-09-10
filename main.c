#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h> //for IOCTL defs
#include <fcntl.h>

//#define I2C_FICHIER "/dev/i2c-2" // fichier Linux representant le BUS #2
//#define I2C_ADRESSE 0x68 // adresse du Device I2C MPU-9250 (motion tracking)


#define I2C_FICHIER "/dev/i2c-1" // fichier Linux representant le BUS #1
#define I2C_ADRESSE 0x29 // adresse du VL6180/satel

int main()
{
	int fdPortI2C;  // file descriptor I2C

	// Initialisation du port I2C, 
	fdPortI2C = open(I2C_FICHIER, O_RDWR); // ouverture du 'fichier', création d'un 'file descriptor' vers le port I2C
	if(fdPortI2C == -1)
	{
		printf("erreur: I2C initialisation step 1\n");
		return -1;
	}
	if(ioctl(fdPortI2C, I2C_SLAVE_FORCE, I2C_ADRESSE) < 0)  // I2C_SLAVE_FORCE if it is already in use by a driver (i2cdetect : UU)
	{
		printf("erreur: I2C initialisation step 2\n");
		close(fdPortI2C);
		return -1;
	}
	
	// Écriture et Lecture sur le port I2C
	//uint8_t Source = 0x75; // registre d'ID du chip I2C
	uint8_t Source[2] = {0x00, 0x00};
	uint8_t Destination;
	uint8_t NombreDOctetsALire = 1;
	uint8_t NombreDOctetsAEcrire = 2;




	if(write(fdPortI2C, &Source, NombreDOctetsAEcrire) != NombreDOctetsAEcrire)
	{
		printf("erreur: Écriture I2C\n");
		close(fdPortI2C);
		return -1;
	}

	if (read(fdPortI2C, &Destination, NombreDOctetsALire) != NombreDOctetsALire)
	{
		printf("erreur: Lecture I2C\n");
		close(fdPortI2C);
		return -1;
	}
	printf("octets lus: %#04x\n", Destination);

	close(fdPortI2C); /// Fermeture du 'file descriptor'
	return 0;
}


