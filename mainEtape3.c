
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h> 
#include <fcntl.h>

#define I2C_FICHIER "/dev/i2c-1" // fichier Linux representant le BUS #2
#define I2C_ADRESSE 0x29 // adresse du Device I2C MPU-9250 (motion tracking)
// Registre et ID du modèle attendu pour le capteur VL6180X
#define VL6180X_ID_REGISTRE 0x000 
#define VL6180X_ID 0xb4         

typedef struct
{
    uint16_t registre;  // Registre à configurer
    uint8_t Valeur; // Valeur de configuration
} VL6180X_Config;

// Configuration initiale 
VL6180X_Config VL6810x_message[] = 
{ 
    {0x0207, 0x01},{0x0208, 0x01},
    {0x0133, 0x01},{0x0096, 0x00},
    {0x0097, 0xFD},{0x00e3, 0x00},
    {0x00e4, 0x04},{0x00e5, 0x02},
    {0x00e6, 0x01},{0x00e7, 0x03},
    {0x00f5, 0x02},{0x00D9, 0x05},
    {0x00DB, 0xCE},{0x00DC, 0x03},
    {0x00DD, 0xF8},{0x009f, 0x00},
    {0x00a3, 0x3c},{0x00b7, 0x00},
    {0x00bb, 0x3c},{0x00b2, 0x09},
    {0x00ca, 0x09},{0x0198, 0x01}, 
};

int initialise_VL6180X(int fd);
int interfaceVL6180x_litUneDistance(int fd); 
int interfaceVL6180x_ecrit(int fd, uint16_t registre, uint8_t Donnee); 
int interfaceVL6180x_lit(int fd, uint16_t registre, uint8_t *Donnee); 


// Fonction initialisation du capteur
int initialise_VL6180X(int fd)
{
    for (int i = 0; i < sizeof(VL6810x_message) / sizeof(VL6180X_Config); i++) 
	{
        uint8_t buffer[3] = { VL6810x_message[i].registre >> 8, 
                              VL6810x_message[i].registre & 0xFF, 
                              VL6810x_message[i].Valeur };
        if (write(fd, buffer, 3) != 3)  
		{
            printf("Erreur: Configuration I2C\n");
            return -1;
        }
    }
    return 0;
}



int main() 
{
	
   int fdPortI2C;// file descriptor I2C
   
  // Initialisation du port I2C, 
   fdPortI2C = open(I2C_FICHIER, O_RDWR);

    if (fdPortI2C == -1) 
	{
        printf("Erreur: I2C initialisation\n");
        return -1;
    }
    
 
    if (ioctl(fdPortI2C, I2C_SLAVE, I2C_ADRESSE) < 0) 
	{
        printf("Erreur: Configuration de l'adresse I2C\n");
        close(fdPortI2C);
        return -1;
    }
    // Écriture et Lecture sur le port I2C 
    uint8_t registreID[2] = {0x00, 0x00}; 
    uint8_t ValeurID;
    //main.c write fdPortI2C modifier
    if (write(fdPortI2C, registreID, 2) != 2 || read(fdPortI2C, &ValeurID, 1) != 1) 
	{
        printf("Erreur: Lecture du registre d'ID\n");
        close(fdPortI2C);
        return -1;
    }
    if (ValeurID != VL6180X_ID)  
	{
        printf("ID incorrect: %#04x\n", ValeurID);
        close(fdPortI2C);
        return -1;
    }
    printf("ID correct: %#04x\n", ValeurID);

    if (initialise_VL6180X(fdPortI2C) != 0) 
	{
        close(fdPortI2C);
        return -1;
    }
    
    if (interfaceVL6180x_litUneDistance(fdPortI2C) != 0) 
	{
        close(fdPortI2C);
        return -1;
    }

	close(fdPortI2C); 
	return 0;
}

// Fonction pour écrire dans un registre spécifique du VL6180X
int interfaceVL6180x_ecrit(int fd, uint16_t registre, uint8_t Donnee) 
{
    uint8_t message[3];
    message[0] = (registre >> 8); 
    message[1] = registre & 0xFF;        
    message[2] = Donnee;                
                                        
    if (write(fd, message, 3) != 3)   
	{
        printf("Erreur: Écriture dans le registre 0x%04X\n", registre);
        return -1;
    }
    return 0;
}


int interfaceVL6180x_lit(int fd, uint16_t registre, uint8_t *Donnee) 
{
    uint8_t buffer[2];
    buffer[0] = (registre >> 8);  
    buffer[1] = registre;         
    
    if (write(fd, buffer, 2) != 2) 
	{
        printf("Erreur: Écriture de l'adresse de registre\n");
        return -1;
    }
	if (read(fd, Donnee, 1) != 1) 
	{
        printf("Erreur: Lecture de la donnée\n");
        return -1;
    }
    return 0;
}


int interfaceVL6180x_litUneDistance(int fd) 
{
uint8_t start_range_cmd[3] = {0x00, 0x18, 0x01}; //(interfaceVL6180x_ecrit(0x18, 0x01) < 0)
    if (write(fd, start_range_cmd, 3) != 3) 
	{
        printf("Erreur: Écriture mesure de distance\n");
        return -1;
    }
    usleep(50000); //lecture finit?
    uint8_t range_register[2] = {0x00, 0x62}; 
    if (write(fd, range_register, 2) != 2) 
	{
        printf("Erreur: Écriture de la lecture \n");
        return -1;
    }

    uint8_t distance;
    if (read(fd, &distance, 1) != 1) 
	{
        printf("Erreur: Lecture de la distance\n");
        return -1;
    }

    printf("Distance = %d \n", distance);
    return 0;
}





