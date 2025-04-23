#pragma warning( disable : 4996 ) 

#include <cstdlib>
#include <vector>
#include <iostream>
#include <thread>
#include <string>
#include "G2D.h"
using namespace std;

// touche P   : mets en pause
// touche ESC : ferme la fenêtre et quitte le jeu


///////////////////////////////////////////////////////////////////////////////
//
//    Données du jeu - structure instanciée dans le main

/*
	Structure d'une hitbox qui est assimilé à un rectangle, Pos est le point en bas à gauche, Size en bas à droite,
	C en haut à droite puis D en haut à gauche
	Nous pouvons créer une Hitbox à l'aide de seulement deux points V2, le premier V2 permet de situer dans l'espace la hitbox,
		le deuxième permet de déterminer sa taille, cela permet de créer des sprites avec de bonnes proportions rapidement
		Le bool IsActivated permet de savoir si on peut calculer la collision dans la fonction CollisionHitbox
*/
struct Background {
	float compteur = 0.0;
	float compteur2 = 0.0;
	int IdFrame = 0;
	int VictoryScreen;
	vector <int> BackgroundList1;
	vector <int> BackgroundList2;
	void InitTextures() {
		BackgroundList1.push_back(G2D::extractTextureFromPPM(".//assets//Background1.ppm", false));
		BackgroundList1.push_back(G2D::extractTextureFromPPM(".//assets//Background2.ppm", false));
		BackgroundList1.push_back(G2D::extractTextureFromPPM(".//assets//Background3.ppm", false));
		BackgroundList1.push_back(G2D::extractTextureFromPPM(".//assets//Background4.ppm", false));
		BackgroundList1.push_back(G2D::extractTextureFromPPM(".//assets//Background5.ppm", false));
		BackgroundList1.push_back(G2D::extractTextureFromPPM(".//assets//Background6.ppm", false));
		BackgroundList1.push_back(G2D::extractTextureFromPPM(".//assets//Background7.ppm", false));

		BackgroundList2.push_back(G2D::extractTextureFromPPM(".//assets//BackgroundBoss1.ppm", false));
		BackgroundList2.push_back(G2D::extractTextureFromPPM(".//assets//BackgroundBoss2.ppm", false));
		BackgroundList2.push_back(G2D::extractTextureFromPPM(".//assets//BackgroundBoss3.ppm", false));
		BackgroundList2.push_back(G2D::extractTextureFromPPM(".//assets//BackgroundBoss4.ppm", false)); 
		VictoryScreen = G2D::extractTextureFromPPM(".//assets//BackgroundVictory.ppm", false);
	}
};
struct Tree {
	V2 Pos;
	V2 Size;
	int IdSpriteTree;
	Tree(V2 _Pos, V2 _Size) {
		Pos = _Pos;
		Size = _Size;
	}
	void InitTextures() {
		IdSpriteTree = G2D::extractTextureFromPPM(".//assets//Tree.ppm", true);
	}
};

struct Hitbox {
	V2 Pos;
	V2 Size;
	V2 C;
	V2 D;
	bool IsActivated = true;
	Hitbox(V2 _Pos, V2 _Size) {
		Pos = _Pos;
		Size = V2(_Pos.x + _Size.x, _Pos.y);
		C = _Pos + _Size;
		D = V2(_Pos.x, _Pos.y + _Size.y);
	}
	Hitbox() {}
};


/*
				Un power up est caractérisé par sa position, sa taille et sa hitbox
				Le bool Collected permet de savoir si le power up a été collecté
				La liste PowerUpList permet de stocker tous les sprites de power up
*/
struct PowerUp {
	V2 Pos;
	V2 Size;
	Hitbox hitbox;
	bool Collected = false;
	vector <int> PowerUpList;
	PowerUp(V2 _Pos, V2 _Size) {
		Pos = _Pos;
		Size = _Size;
		hitbox = Hitbox(Pos, Size);
	}
	void InitTextures()
	{
		PowerUpList.push_back(G2D::extractTextureFromPPM(".//assets//Green_Emerald.ppm", true));
	}
};

struct Teleport {
	V2 Pos;
	V2 Size;
	Hitbox hitbox = Hitbox(Pos, Size);
	vector <int> TeleportList;
	Teleport(V2 _Pos, V2 _Size) {
		Pos = _Pos;
		Size = _Size;
		hitbox = Hitbox(Pos, Size);
	}
	void InitTextures()
	{
		TeleportList.push_back(G2D::extractTextureFromPPM(".//assets//Chakroad.ppm", true));
	}
};
/*
				Une plateforme est caractérisé par sa position, sa taille et son sprite, il n'est pas initialisé au début afin de choisir quel sprite appliquer
				sur la plateforme, il faut donc le faire lors de l'affichage des différentes plateformes dans la fonction render
*/
struct Plateforme {
	float FallCounter = 0.0;
	V2 Pos;
	V2 InitialPos;
	V2 Size;
	V2 Next_move;
	Hitbox hitbox = Hitbox(Pos, Size);
	vector <int> PlatformList;
	Plateforme(V2 _Pos, V2 _Size) {
		Pos = _Pos;
		Size = _Size;
		InitialPos = _Pos;
		hitbox = Hitbox(Pos, Size);
		Next_move = V2(0, 0);
	}

	void FallingPlatform() {
		FallCounter = FallCounter + 1;
		if (FallCounter < 200) {
			Next_move = V2(0, -1);
		}
		else {
			Pos = InitialPos;
			Next_move = V2(0, 0);
			FallCounter = 0;
		}
	}

	void InitTextures()
	{
		PlatformList.push_back(G2D::extractTextureFromPPM(".//assets//platform1.ppm", false));
		PlatformList.push_back(G2D::extractTextureFromPPM(".//assets//BossPlatform.ppm", true));
	}
};

struct Boss
{
	int health = 5;
	bool IsEnabled = false;
	bool IsImmuned = false;
	float CantBeAttacked = 0.0;
	float ClapCounter = 0.0;
	V2 Pos;
	V2 LeftHandPos;
	V2 RightHandPos;
	V2 Size;
	V2 LeftHandSize = V2(66, 150);
	V2 RightHandSize = V2(66, 150);
	V2 Next_move = V2(0, 0);
	V2 LeftHandNext_move = V2(0, 0);
	V2 RightHandNext_move = V2(0, 0);
	Hitbox BossHitbox;
	Hitbox LeftHandHitbox;
	Hitbox RightHandHitbox;
	vector <int> ListSprite;
	

	Boss(V2 _Pos, V2 _Size, V2 _Next_move) {
		Pos = _Pos;
		Size = _Size;
		Next_move = _Next_move;
		BossHitbox = Hitbox(Pos, Size);
		LeftHandPos = V2(Pos.x - 430, Pos.y - 300);
		RightHandPos = V2(Pos.x + 460, Pos.y - 300);
		LeftHandHitbox = Hitbox(LeftHandPos, LeftHandSize);
		RightHandHitbox = Hitbox(RightHandPos, RightHandSize);
	}
	void IsImmune() {
		if (CantBeAttacked > 5) CantBeAttacked = 0;
		CantBeAttacked = CantBeAttacked + 0.25;
	}
	void Disable() {
		IsEnabled = false;
		Next_move = V2(0, -2);
		LeftHandNext_move = V2(0, -2);
		RightHandNext_move = V2(0, -2);
		LeftHandHitbox.IsActivated = false;
		RightHandHitbox.IsActivated = false;
	}
	void Clap() {
		ClapCounter = ClapCounter + 1;
		if (ClapCounter < 60) {
			LeftHandNext_move = V2(7, 0);
			RightHandNext_move = V2(-7, 0);
		}
		else if (ClapCounter < 118) {
			LeftHandNext_move = V2(-7, 0);
			RightHandNext_move = V2(7, 0);
		}
		else {
			ClapCounter = 0;
		}
	}



	void InitTextures()
	{
		ListSprite.push_back(G2D::extractTextureFromPPM(".//assets//EntireBoss.ppm", true));
		ListSprite.push_back(G2D::extractTextureFromPPM(".//assets//BossLeftHand.ppm", true));
		ListSprite.push_back(G2D::extractTextureFromPPM(".//assets//BossRightHand.ppm", true));
	}
};

struct Bumper {
	V2 Pos;
	V2 Size;
	bool active;
	Hitbox hitbox = Hitbox(Pos, Size);
	vector <int> BumperList;

	Bumper(V2 _Pos, V2 _Size) {
		Pos = _Pos;
		Size = _Size;
		hitbox = Hitbox(Pos, Size);
		active = false;
	}

	void InitTextures()
	{
		BumperList.push_back(G2D::extractTextureFromPPM(".//assets//BumperRelaxed.ppm", true));
		BumperList.push_back(G2D::extractTextureFromPPM(".//assets//BumperLong.ppm", true));
	}

};

/*
		Un projectile est caractérisé par sa position, sa taille, son sprite mais aussi sa vitesse et son point de départ puisqu'il peut être tiré
		plusieurs fois, les attaques à distance nécessitant des projectiles se feront à l'aide de la fonction IsShooted, tandis que les attaques au
		corps-à-corps se feront à l'aide de fonctions dans la structure Monster
		Le bool IsEnabled permet de savoir si le projectile est encore à l'écran
		Le bool FromEnnemy permet de savoir si le projectile provient d'un ennemi ou de Sonic
*/
struct Projectile {
	bool IsEnabled;
	bool FromEnnemy;
	bool ShootedFromLeft;
	V2 Pos;
	V2 Size;
	V2 Speed;
	V2 StartingPoint;
	Hitbox hitbox;
	vector <int> ProjectileList;
	Projectile(V2 _Pos, V2 _Size, bool _IsEnabled, bool _FromEnnemy) {
		Pos = _Pos;
		Size = _Size;
		IsEnabled = _IsEnabled;
		FromEnnemy = _FromEnnemy;
		StartingPoint = _Pos;
		hitbox = Hitbox(Pos, Size);
	}
	Projectile() {};
	void IsShooted(V2 speed) {
		Speed = speed;
		Pos = Pos + speed;
	}
	void SetStartingPoint(V2 NewStartingPoint) {
		StartingPoint = NewStartingPoint;
	}
	void SetPos(V2 NewPos) {
		Pos = NewPos;
	}
	void AddSize(V2 NewSize) {
		Size = Size + NewSize;
	}
	void AddPos(V2 NewPos) {
		Pos = Pos + NewPos;
	}
	void Disable(bool IsEnabled) {
		Size = V2(0, 0);
		Pos = V2(0, 0);
	}
	void InitTextures()
	{
		ProjectileList.push_back(G2D::extractTextureFromPPM(".//assets//Missile.ppm", true));
		ProjectileList.push_back(G2D::extractTextureFromPPM(".//assets//Kameha.ppm", true));
	}
};

/*
	La structure Character intègre maintenant une varibale Hitbox pour le Character correspondant à sa position et à la taile de son sprite
		Next_move permet de décrire la direction du Character
		Stable permet de savoir si le Character se trouve actuellement sur une plateforme
		Les listes CourseGauche et CourseDroite permettent de stocker tous les sprites afin de créer une animation
		Compteur permet de savoir à quel moment il faut reboucler l'animation
		SuperList contient tous les sprites de Sonic lorsqu'il est en Super Sonic

	La fonction InitTextures permet d'initialiser toutes les textures d'un coup pour le Characters
		Le bool Super permet de savoir si Sonic est en version Super
		Le bool KamehaState permet de savoir si Sonic est en train de charger un kameha
		Le bool Left permet de savoir si Sonic regarde vers la gauche ou la droite
*/

struct Character
{
	V2 Pos;
	V2 Size;
	V2 Next_move;
	V2 Camera = V2(0, 0);
	Hitbox hitbox;
	vector <int> CourseGauche;
	vector <int> CourseDroite;
	vector <int> JumpLeft;
	vector <int> JumpRight;
	vector <int> SuperList;
	bool FightAgainstBoss = false;
	bool bouge;
	bool Stable = true;
	bool Super = false;
	bool KamehaState = false;
	bool Left = false;
	bool IsOnCooldown = false;
	float compteur = 0.0;
	float JumpCounter = 0.0;
	int IdSpriteNormal;
	int IdSpriteLeft;
	int IdSpriteRight;
	int IdAirRight;
	int IdAirLeft;
	int IdSlideRight;
	int IdSlideLeft;
	int IdKamehaRight;
	int IdKamehaLeft;

	Character(V2 _Pos, V2 _Size, V2 _Next_move) {
		Pos = _Pos;
		Size = _Size;
		Next_move = _Next_move;
		hitbox = Hitbox(Pos, Size);
	}

	void InitTextures()
	{
		IdSpriteNormal = G2D::extractTextureFromPPM(".//assets//SonicBaseDroite.ppm", true);
		IdSpriteLeft = G2D::extractTextureFromPPM(".//assets//SonicBaseGauche.ppm", true);
		IdSpriteRight = G2D::extractTextureFromPPM(".//assets//SonicBaseDroite.ppm", true);
		IdAirLeft = G2D::extractTextureFromPPM(".//assets//SonicBaseAirGauche.ppm", true);
		IdAirRight = G2D::extractTextureFromPPM(".//assets//SonicBaseAirDroite.ppm", true);
		IdSlideLeft = G2D::extractTextureFromPPM(".//assets//SonicBaseGlisseGauche.ppm", true);
		IdSlideRight = G2D::extractTextureFromPPM(".//assets//SonicBaseGlisseDroite.ppm", true);
		IdKamehaRight = G2D::extractTextureFromPPM(".//assets//SuperSonicKamehaDroite.ppm", true);
		IdKamehaLeft = G2D::extractTextureFromPPM(".//assets//SuperSonicKamehaGauche.ppm", true);
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite1.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite2.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite3.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite4.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite5.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite6.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite7.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite8.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite9.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite10.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite11.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite12.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite13.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite14.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite15.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite16.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite17.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite18.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite19.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite20.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite21.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite22.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite23.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite24.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite25.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite26.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite27.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite28.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite29.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite30.ppm", true));
		CourseDroite.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseDroite31.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche1.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche2.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche3.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche4.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche5.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche6.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche7.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche8.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche9.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche10.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche11.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche12.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche13.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche14.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche15.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche16.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche17.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche18.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche19.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche20.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche21.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche22.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche23.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche24.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche25.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche26.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche27.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche28.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche29.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche30.ppm", true));
		CourseGauche.push_back(G2D::extractTextureFromPPM(".//assets//SonicBaseGauche31.ppm", true));
		SuperList.push_back(G2D::extractTextureFromPPM(".//assets//super_sonic_base.ppm", true));
		SuperList.push_back(G2D::extractTextureFromPPM(".//assets//super_sonic_gauche.ppm", true));
		SuperList.push_back(G2D::extractTextureFromPPM(".//assets//super_sonic_droite.ppm", true));
		SuperList.push_back(G2D::extractTextureFromPPM(".//assets//SuperSonicBaseGauche.ppm", true));
		JumpLeft.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpLeft1.ppm", true));
		JumpLeft.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpLeft2.ppm", true));
		JumpLeft.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpLeft3.ppm", true));
		JumpLeft.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpLeft4.ppm", true));
		JumpLeft.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpLeft5.ppm", true));
		JumpLeft.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpLeft6.ppm", true));
		JumpLeft.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpLeft7.ppm", true));
		JumpLeft.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpLeft8.ppm", true));
		JumpRight.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpRight1.ppm", true));
		JumpRight.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpRight2.ppm", true));
		JumpRight.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpRight3.ppm", true));
		JumpRight.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpRight4.ppm", true));
		JumpRight.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpRight5.ppm", true));
		JumpRight.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpRight6.ppm", true));
		JumpRight.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpRight7.ppm", true));
		JumpRight.push_back(G2D::extractTextureFromPPM(".//assets//SonicJumpRight8.ppm", true));
	}
	//Permet à Sonic de charger une attaque, il ne bouge plus donc next_move est nul et la gravité ne s'applique pas
	void Kameha(bool IsAttacking) {
		if (IsAttacking) {
			Next_move = V2(0, 0);
			KamehaState = true;
		}
		else {
			KamehaState = false;
		}
	}

	//Permet de savoir si Sonic peut relancer une attaque
	void SetIsOnCooldown(bool _IsOnCooldown) {
		IsOnCooldown = _IsOnCooldown;
	}

	//Permet à Sonic de réapparaître si il tombe ou est touché par des ennemis, il perd ses bonus
	void Respawn() {
		Next_move = V2(0, 0);
		compteur = 0.0;
		if (FightAgainstBoss) {
			Pos = V2(9400, 450);
		}
		else {
			Pos = V2(50, 450);
		}
		if (Super) Super = false;
	}



	void Bouge() {

		if (compteur > 30) compteur = 25;
		if (compteur < -30) compteur = -25;
		//Sonic peut sauter depuis une plateforme
		if (G2D::isKeyPressed(Key::UP) && Stable) {
			if (Super) {
				Next_move.y = 17;
				Size = V2(60, 80);
			}
			else {
				Next_move.y = 12;
				Size = V2(58, 60);
			}

		}

		//Sonic peut se déplacer vers la droite si il ne charge pas une attaque
		else if (G2D::isKeyPressed(Key::RIGHT) && !KamehaState) {
			Left = false;
			if (Super) {
				Size = V2(92, 64);
				Next_move.x = 20;
			}
			else {
				if (Stable) {
					Size = V2(60, 80);
					//Si la vitesse de Sonic est en dessous de 30, la vitesse augmente de 0.25
					if (Next_move.x < 30) {
						//Si Sonic se déplace dans l'autre sens et qu'il y a de la friction, il gagne un bonus de vitesse tant qu'il ne va pas dans la bonne direction
						if (Next_move.x < 0) {
							Next_move.x = Next_move.x + 0.5;
							compteur = compteur + 0.5;
						}
						Next_move.x = Next_move.x + 0.25;
						compteur = compteur + 0.25;
						if (compteur > 10) Size = V2(65, 80);
						if (compteur > 24) Size = V2(75, 70);
					}
					//Sinon, la vitesse est bloqué, mais le compteur continue de compter afin de reboucler l'animation
					else {
						Size = V2(75, 70);
						Next_move.x = Next_move.x;
						if (compteur < 30) {
							compteur = compteur + 0.25;
						}
						else {
							compteur = 25;
						}
					}
				}
				else {
					Size = V2(50, 55);
					if (Next_move.x < 30) {
						//Si Sonic se déplace dans l'autre sens et qu'il y a de la friction, il gagne un bonus de vitesse tant qu'il ne va pas dans la bonne direction
						if (Next_move.x < 0) {
							Next_move.x = Next_move.x + 0.5;
							compteur = compteur + 0.5;
						}
						Next_move.x = Next_move.x + 0.25;
						compteur = compteur + 0.25;
					}
					//Sinon, la vitesse est bloqué, mais le compteur continue de compter afin de reboucler l'animation
					else {
						Next_move.x = Next_move.x;
						if (compteur < 30) {
							compteur = compteur + 0.25;
						}
						else {
							compteur = 25;
						}
					}
				}
			}
		}

		else if (G2D::isKeyPressed(Key::LEFT) && !KamehaState) {
			Left = true;
			if (Super) {
				Size = V2(92, 64);
				Next_move.x = -20;
			}
			else {
				if (Stable) {
					Size = V2(60, 80);
					if (Next_move.x > -30) {
						if (Next_move.x > 0) {
							Next_move.x = Next_move.x - 0.5;
							compteur = compteur - 0.5;
						}
						Next_move.x = Next_move.x - 0.25;
						compteur = compteur - 0.25;
						if (compteur < -10) Size = V2(65, 80);
						if (compteur < -24) Size = V2(75, 70);
					}
					else {
						Size = V2(75, 70);
						Next_move.x = Next_move.x;
						if (compteur > -30) {
							compteur = compteur - 0.25;
						}
						else {
							compteur = -25;
						}
					}
				}
				else {
					Size = V2(50, 55);
					if (Next_move.x > -30) {
						if (Next_move.x > 0) {
							Next_move.x = Next_move.x - 0.5;
							compteur = compteur - 0.5;
						}
						Next_move.x = Next_move.x - 0.25;
						compteur = compteur - 0.25;
					}
					else {
						Next_move.x = Next_move.x;
						if (compteur > -30) {
							compteur = compteur - 0.25;
						}
						else {
							compteur = -25;
						}
					}
				}
			}
		}


		//Si Sonic se déplace alors que le joueur ne touche à rien, alors c'est qu'il glisse à cause des frictions
		else if (abs(Next_move.x) > 0.1 && Stable && !(G2D::isKeyPressed(Key::RIGHT) || G2D::isKeyPressed(Key::LEFT))) {
			if (Super) Size = V2(60, 80);
			else Size = V2(69, 75);
		}
		else {
			if (Stable) {
				if (Super) Size = V2(60, 80);
				else Size = V2(60, 80);
			}
			else {
				if (Super) Size = V2(60, 80);
				else Size = V2(50, 55);
			}
		}

		//Animation de Sonic quand il saute
		JumpCounter = JumpCounter + 0.25;
		if (JumpCounter == 8) {
			JumpCounter = 0;
		}

	}
};

/*
		La structure Monster est caractérisé par sa position, sa taille et son sprite, comme nous voulons avoir des Monsters différents,
		le sprite n'est pas initialisé et sera affecté dans la fonction render qui pourra choisir le sprite adéquat dans la liste
		Ennemy1
*/

struct Monster {
	V2 Pos;
	V2 Size;
	V2 Speed = V2(0, 0);
	Hitbox hitbox;
	vector <int> Ennemy1;
	vector <int> Ennemy2;
	float compteur = 0.0;


	Monster(V2 _Pos, V2 _Size) {
		Pos = _Pos;
		Size = _Size;
		hitbox = Hitbox(Pos, Size);
	}

	void Disable() {
		Size = V2(0, 0);
		Pos = V2(0, 0);
	}

	void InitTextures()
	{
		Ennemy1.push_back(G2D::extractTextureFromPPM(".//assets//Ennemi1.ppm", true));
		Ennemy2.push_back(G2D::extractTextureFromPPM(".//assets//Ennemi2Left.ppm", true));
		Ennemy2.push_back(G2D::extractTextureFromPPM(".//assets//Ennemi2Left2.ppm", true));
		Ennemy2.push_back(G2D::extractTextureFromPPM(".//assets//Ennemi2Right.ppm", true));
		Ennemy2.push_back(G2D::extractTextureFromPPM(".//assets//Ennemi2Right2.ppm", true));
	}
};
/*
		L'objet GameData permet de créer le jeu, il ne sera crée qu'une seule fois, il est caractérisé par la taille de sa fenêtre, du nombre de frames
		mais surtout de tous les objets qui sont crées dans le jeu
*/

struct GameData
{
	int HeighPix = 800;   // hauteur de la fenêtre d'application
	int WidthPix = 1200;   // largeur de la fenêtre d'application

	int idFrame = 0;
	//création du Character
	Character Sonic = Character(V2(WidthPix / 2, 370), V2(60, 80), V2(0, 0));
	Background background;
	Boss Eggman = Boss(V2(10350, -150), V2(114, 126), V2(0, 0));
	//Liste des plateformes
	vector<Plateforme> Pf{
		Plateforme(V2(0,0),V2(300,300)),
		Plateforme(V2(300,0),V2(300,300)),
		Plateforme(V2(600,0),V2(300,300)),
		Plateforme(V2(900,0),V2(300,300)),
		Plateforme(V2(1200,0),V2(300,300)),
		Plateforme(V2(1500,0),V2(300,300)),
		Plateforme(V2(1800,0),V2(300,300)),
		Plateforme(V2(2100,0),V2(300,300)),
		Plateforme(V2(200,450),V2(300,40)),
		Plateforme(V2(480,450),V2(300,40)),
		Plateforme(V2(750,450),V2(300,40)),
		Plateforme(V2(990,450),V2(300,40)),
		Plateforme(V2(0,640),V2(275,40)),
		Plateforme(V2(255,830),V2(300,40)),
		Plateforme(V2(-100,970),V2(300,40)),
		Plateforme(V2(430,1120),V2(300,40)),
		Plateforme(V2(550,1120),V2(300,40)),
		Plateforme(V2(750,1120),V2(300,40)),
		Plateforme(V2(1010,1120),V2(300,40)),
		Plateforme(V2(1260,1120),V2(300,40)),
		Plateforme(V2(1260,1120),V2(300,40)),
		Plateforme(V2(1400,770),V2(300,40)),
		Plateforme(V2(2000,970),V2(300,40)),
		Plateforme(V2(-100,1650),V2(300,40)),
		Plateforme(V2(50,1650),V2(300,40)),
		Plateforme(V2(300,1650),V2(300,40)),
		Plateforme(V2(450,1650),V2(300,40)),
		Plateforme(V2(650,1650),V2(300,40)),
		Plateforme(V2(1200,1650),V2(300,40)),
		Plateforme(V2(1460,1650),V2(300,40)),
		Plateforme(V2(1610,1650),V2(300,40)),
		Plateforme(V2(400,1850),V2(300,40)),
		Plateforme(V2(690,2050),V2(300,40)),
		Plateforme(V2(980,2250),V2(300,40)),
		Plateforme(V2(1250,2750),V2(300,40)),
		Plateforme(V2(1500,2750),V2(300,40)),
		Plateforme(V2(1750,2750),V2(300,40)),
		Plateforme(V2(0,2950),V2(300,40)),
		Plateforme(V2(250,2950),V2(300,40)),
		Plateforme(V2(500,2950),V2(300,40)),
		Plateforme(V2(750,2950),V2(300,40)),
		Plateforme(V2(1280,3150),V2(300,40)),
		Plateforme(V2(1500,3150),V2(300,40)),
		Plateforme(V2(1650,3630),V2(300,40)),
		Plateforme(V2(1900,3630),V2(300,40)),
		Plateforme(V2(2150,3630),V2(300,40)),
		Plateforme(V2(2300,3630),V2(300,40)),
		Plateforme(V2(2550,3630),V2(300,40)),
		Plateforme(V2(1080,3630),V2(300,40)),

		Plateforme(V2(960,3630),V2(300,40)),
		Plateforme(V2(710,3630),V2(300,40)),
		Plateforme(V2(460,3630),V2(300,40)),
		Plateforme(V2(210,3630),V2(300,40)),
		Plateforme(V2(621,3830),V2(300,40)),
		Plateforme(V2(2490,3830),V2(300,40)),
		Plateforme(V2(900,4020),V2(300,40)),
		Plateforme(V2(1200,4020),V2(300,40)),
		Plateforme(V2(1400,4020),V2(300,40)),
		Plateforme(V2(1600,4020),V2(300,40)),
		Plateforme(V2(1800,4020),V2(300,40)),
		Plateforme(V2(2000,4020),V2(300,40)),
	};
	vector<Plateforme> PfBoss{
		Plateforme(V2(9400,180),V2(280,120)),
		Plateforme(V2(9780,180),V2(280,120)),
		Plateforme(V2(10080,180),V2(280,120)),
		Plateforme(V2(10380,180),V2(280,120)),
		Plateforme(V2(10680,180),V2(280,120)),
	};
	//Liste des bumpers
	vector<Bumper> Bp{
		Bumper(V2(1790,300), V2(56,42)),
		Bumper(V2(1050,1160), V2(56,42)),
		Bumper(V2(1150,2290), V2(56,42)),
		Bumper(V2(1508,3190), V2(56,42)),
		Bumper(V2(1600,4060), V2(56,42)),
		Bumper(V2(10820,300), V2(56,42)),
	};
	//Listes des Ennemis
	vector<Monster> Ennemis{
		//Monsieur pistolet
		Monster(V2(1200,490),V2(54,40)),
		Monster(V2(1700,1160),V2(54,40)),
		Monster(V2(900,1890),V2(54,40)),
		Monster(V2(1150,2090),V2(54,40)),
		Monster(V2(1200,2190),V2(54,40)),
		Monster(V2(2100,2790),V2(54,40)),
		Monster(V2(1900,3190),V2(54,40)),
		Monster(V2(2000,3290),V2(54,40)),
		Monster(V2(2100,3390),V2(54,40)),
		Monster(V2(2200,3490),V2(54,40)),
		// Monsieur cochon
		Monster(V2(1200,300),V2(60,38)),
		Monster(V2(1700,1690),V2(60,38)),
		Monster(V2(1700,3670),V2(60,38)),
	};
	vector<Projectile> Projectiles{
		Projectile(V2(Ennemis[0].Pos),V2(22,26), true, true),
		Projectile(V2(Ennemis[1].Pos),V2(22,26), true, true),
		Projectile(V2(Ennemis[2].Pos),V2(22,26), true, true),
		Projectile(V2(Ennemis[3].Pos),V2(22,26), true, true),
		Projectile(V2(Ennemis[4].Pos),V2(22,26), true, true),
		Projectile(V2(Ennemis[5].Pos),V2(22,26), true, true),
		Projectile(V2(Ennemis[6].Pos),V2(22,26), true, true),
		Projectile(V2(Ennemis[7].Pos),V2(22,26), true, true),
		Projectile(V2(Ennemis[8].Pos),V2(22,26), true, true),
		Projectile(V2(Ennemis[9].Pos),V2(22,26), true, true),
		Projectile(V2(0,0),V2(0,0), false, false),
	};
	vector<PowerUp> PowerUps{
		PowerUp(V2(600,500),V2(27,24)),
		PowerUp(V2(300,870),V2(27,24)),
		PowerUp(V2(200,1740),V2(27,24)),
		PowerUp(V2(100,3040),V2(27,24)),
		PowerUp(V2(750,3970),V2(27,24)),
		PowerUp(V2(2590,3970),V2(27,24)),
	};
	vector<Tree> Trees{
		Tree(V2(200,HeighPix / 12),V2(91,104)),
		Tree(V2(500,HeighPix / 8),V2(91,104)),
		Tree(V2(900,HeighPix / 11),V2(91,104)),
		Tree(V2(1200,HeighPix / 7),V2(91,104)) 
	};
	vector<Teleport> Teleports{
			 Teleport(V2(1600,4500), V2(60,60)),
	};
	GameData() {}

};


///////////////////////////////////////////////////////////////////////////////
//
// 
//     fonction de rendu - reçoit en paramètre les données du jeu par référence

//Permet de savoir si le joueur touche à une touche de déplacement
bool Moving() {
	if (G2D::isKeyPressed(Key::RIGHT) || G2D::isKeyPressed(Key::LEFT) || G2D::isKeyPressed(Key::UP) || G2D::isKeyPressed(Key::DOWN)) {
		return true;
	}
	else {
		return false;
	}
}

V2 NewReferentiel(V2 InitialPos, V2 decalage) {
	return (V2(InitialPos.x - decalage.x, InitialPos.y - decalage.y));
}

//Permet d'afficher les délimitations d'une hitbox
void ShowingHitbox(GameData& G, Hitbox HitboxToShow) {
	G2D::drawLine(HitboxToShow.Pos + G.Sonic.Camera, HitboxToShow.Size, Color::Red);
	G2D::drawLine(HitboxToShow.Size + G.Sonic.Camera, HitboxToShow.C, Color::Red);
	G2D::drawLine(HitboxToShow.C + G.Sonic.Camera, HitboxToShow.D, Color::Red);
	G2D::drawLine(HitboxToShow.D + G.Sonic.Camera, HitboxToShow.Pos, Color::Red);
}

/*
	Vérifie si deux Hitbox (rectangle) se chevauchent, si c'est le cas, return true
*/
int CollisionHitbox(Hitbox hitbox1, Hitbox hitbox2)
{
	if (hitbox1.C.x < hitbox2.Pos.x || hitbox1.Pos.x > hitbox2.C.x) {
		return 0;
	}
	if (hitbox1.C.y < hitbox2.Pos.y || hitbox1.Pos.y > hitbox2.C.y) {
		return 0;
	}
	// Check if one box is fully inside the other
	/*if (hitbox1.Pos.x >= hitbox2.Pos.x && hitbox1.Pos.y >= hitbox2.Pos.y && hitbox1.C.x <= hitbox2.C.x && hitbox1.C.y <= hitbox2.C.y) {
		//cout << "dedans";
		return 2;
	}
	if (hitbox2.Pos.x >= hitbox1.Pos.x && hitbox2.Pos.y >= hitbox1.Pos.y && hitbox2.C.x <= hitbox1.C.x && hitbox2.C.y <= hitbox1.C.y) {
		//cout << "dedans";
		return 2;
	}*/
	// Check if there is an intersection on the bottom
	if (hitbox1.Pos.y <= hitbox2.C.y && hitbox1.Pos.y + 10 >= hitbox2.C.y && hitbox1.C.y >= hitbox2.C.y) {
		return 1;
	}
	// Check if there is an intersection on top
	if (hitbox1.D.y > hitbox2.Pos.y && hitbox1.Pos.y < hitbox2.Pos.y && (hitbox2.Pos.x < hitbox1.D.x < hitbox2.Size.x || hitbox2.Pos.x < hitbox1.C.x < hitbox2.Size.x)) {
		cout << "en haut ";
		return 2;
	}
	// Check if there is an intersection on right
	if (hitbox1.Size.x > hitbox2.Pos.x && hitbox2.Size.x > hitbox1.Size.x && (hitbox2.Pos.y < hitbox1.Size.y < hitbox2.D.y || hitbox2.Pos.y < hitbox1.D.y < hitbox2.D.y)) {
		cout << "droit ";
		return 3;
	}
	// Check if there is an intersection on left
	if (hitbox1.Pos.x < hitbox2.Size.x && hitbox1.Size.x > hitbox2.Size.x && (hitbox2.Pos.y < hitbox1.Size.y < hitbox2.D.y || hitbox2.Pos.y < hitbox1.D.y < hitbox2.D.y)) {
		cout << "gauche ";
		return 4;
	}



	// Otherwise, they are not colliding
	return 0;
}

void Render(const GameData& G)
{
	V2 decalage = G.Sonic.Camera;
	// fond noir
	G2D::clearScreen(Color::Black);

	// affiche du texte
	if (G2D::isOnPause())
		G2D::drawStringFontMono(V2(50, 400), "Pause", 60, 3, Color::Green);

	if (!G.Sonic.FightAgainstBoss) G2D::drawRectWithTexture(G.background.BackgroundList1[G.background.compteur], NewReferentiel(V2(-100, -5), decalage * 0.01), V2(G.WidthPix * 1.5, G.HeighPix * 1.5));
	else G2D::drawRectWithTexture(G.background.BackgroundList2[G.background.IdFrame], NewReferentiel(V2(-100, -5), decalage * 0.01), V2(G.WidthPix * 1.3, G.HeighPix * 1.2));

	for (int i = 0; i < G.Trees.size(); i++) {
		int IdSpriteTree = G.Trees[i].IdSpriteTree;
		G2D::drawRectWithTexture(IdSpriteTree, NewReferentiel(G.Trees[i].Pos, decalage * 0.5), G.Trees[i].Size * 5);
	}


	// affiche du texte
	//G2D::drawStringFontMono(V2(50, 700), "Hello World !", 20, 3, Color::Red);

	// recupère les coordonnées de la souris
	int x, y;
	G2D::getMousePos(x, y);
	V2 TrueHitbox = V2(-270, -300);
	G2D::drawRectWithTexture(G.Eggman.ListSprite[0], NewReferentiel(G.Eggman.Pos + TrueHitbox, decalage), V2(654, 483));
	G2D::drawRectWithTexture(G.Eggman.ListSprite[1], NewReferentiel(G.Eggman.LeftHandPos, decalage), V2(127, 174));
	G2D::drawRectWithTexture(G.Eggman.ListSprite[2], NewReferentiel(V2(G.Eggman.RightHandPos.x -63, G.Eggman.RightHandPos.y), decalage), V2(127, 174));

	/*
	Affiche les plateformes, pour les afficher, il faut procéder bloc par bloc, vu que les différents sprites sont stockés dans la liste PlatformList
	dans la structure plateforme, il faut sélectionner le sprite que l'on souhaite et l'appliquer à tous les blocs qui doivent le recevoir
	*/

	for (int i = 0; i < G.Pf.size(); i++) {
		int pfsprite1 = G.Pf[i].PlatformList[0];
		G2D::drawRectWithTexture(pfsprite1, NewReferentiel(G.Pf[i].Pos, decalage), G.Pf[i].Size);
	}

	for (int i = 0; i < G.PfBoss.size(); i++) {
		int pfsprite2 = G.PfBoss[i].PlatformList[1];
		G2D::drawRectWithTexture(pfsprite2, NewReferentiel(G.PfBoss[i].Pos, decalage), G.PfBoss[i].Size);
	}
	for (int i = 0; i < G.Teleports.size(); i++) {
		int Telesprite1 = G.Teleports[i].TeleportList[0];
		G2D::drawRectWithTexture(Telesprite1, NewReferentiel(G.Teleports[i].Pos, decalage), G.Teleports[i].Size);
	}
	for (int i = 0; i < G.Bp.size(); i++) {
		int BpShort = G.Bp[i].BumperList[0];
		int BpLong = G.Bp[i].BumperList[1];
		if (G.Bp[i].active)G2D::drawRectWithTexture(BpLong, NewReferentiel(G.Bp[i].Pos, decalage), G.Bp[i].Size);
		else G2D::drawRectWithTexture(BpShort, NewReferentiel(G.Bp[i].Pos, decalage), G.Bp[i].Size);
	}

	//affiche les projectiles actifs, il faut choisir le bon sprite dans ProjectileList
	for (int i = 0; i < G.Projectiles.size(); i++) {
		int missilesprite = G.Projectiles[i].ProjectileList[0];
		if (G.Projectiles[i].FromEnnemy) G2D::drawRectWithTexture(missilesprite, NewReferentiel(G.Projectiles[i].Pos, decalage), G.Projectiles[i].Size);
		int KamehaSprite = G.Projectiles[10].ProjectileList[1];
		if (G.Projectiles[10].IsEnabled) G2D::drawRectWithTexture(KamehaSprite, NewReferentiel(G.Projectiles[10].Pos, decalage), G.Projectiles[10].Size);
	}

	//affiche les ennemis, il faut choisir le bon sprite dans Ennemy1
	for (int i = 0; i < 10; i++) {
		int ennemisprite = G.Ennemis[i].Ennemy1[0];
		G2D::drawRectWithTexture(ennemisprite, NewReferentiel(G.Ennemis[i].Pos, decalage), G.Ennemis[i].Size);
	}
	for (int i = 10; i < G.Ennemis.size(); i++) {

		if (G.Ennemis[i].Speed.x < 0) G2D::drawRectWithTexture(G.Ennemis[i].Ennemy2[G.Ennemis[i].compteur], NewReferentiel(G.Ennemis[i].Pos, decalage), G.Ennemis[i].Size);
		else G2D::drawRectWithTexture(G.Ennemis[i].Ennemy2[G.Ennemis[i].compteur + 2], NewReferentiel(G.Ennemis[i].Pos, decalage), G.Ennemis[i].Size);
	}
	//affiche les power up qui ne sont pas encore collecté
	for (int i = 0; i < G.PowerUps.size(); i++) {
		int pusprite = G.PowerUps[i].PowerUpList[0];
		if (!G.PowerUps[i].Collected)G2D::drawRectWithTexture(pusprite, NewReferentiel(G.PowerUps[i].Pos, decalage), G.PowerUps[i].Size);
	}

	// affiche le sprite selon les déplacements
	int idsprite = G.Sonic.IdSpriteNormal;
	int vitesse = G.Sonic.compteur;

	//Animation lorsque Sonic est sur une plateforme
	if (G.Sonic.Stable) {
		//Sprites différents si il est en version Super
		if (G.Sonic.Super) {
			if (G.Sonic.Next_move.x < -0.1 && Moving()) {
				idsprite = G.Sonic.SuperList[1];
			}
			else if (G.Sonic.Next_move.x > 0.1 && Moving()) {
				idsprite = G.Sonic.SuperList[2];
			}
			else if (G.Sonic.Left && G.Sonic.Super) {
				idsprite = G.Sonic.SuperList[3];
			}
			else {
				idsprite = G.Sonic.SuperList[0];
			}
		}
		//Animation lorsque c'est le sonic de base
		else {
			//Si il regarge à gauche et qu'il ne bouge pas le sprite sera celui de gauche
			if (G.Sonic.Left && !G.Sonic.Super && !Moving()) idsprite = G.Sonic.IdSpriteLeft;
			//Animation lorsque Sonic se déplace vers la gauche car le joueur appuie sur la touche de déplacement,
			if (G.Sonic.Next_move.x < 0 && Moving()) {
				if (G2D::isKeyPressed(Key::RIGHT) && !G2D::isKeyPressed(Key::LEFT)) idsprite = G.Sonic.IdSlideLeft;
				else if (vitesse < -0.1 && vitesse > -31) idsprite = G.Sonic.CourseGauche[-vitesse];
			}
			//Sinon, il court vers la droite
			else if (G.Sonic.Next_move.x > 0 && Moving()) {
				if (!G2D::isKeyPressed(Key::RIGHT) && G2D::isKeyPressed(Key::LEFT)) idsprite = G.Sonic.IdSlideRight;
				else if (vitesse > 0.1 && vitesse < 31) idsprite = G.Sonic.CourseDroite[vitesse];
			}
			//Si Sonic se déplace vers la gauche alors que le joueur ne touche à rien, il effectue une glissade
			else if (G.Sonic.Next_move.x < -0.1 && !Moving()) {
				idsprite = G.Sonic.IdSlideLeft;
			}
			//Glissade vers la droite
			else if (G.Sonic.Next_move.x > 0.1 && !Moving()) {
				idsprite = G.Sonic.IdSlideRight;
			}
		}

	}
	//Sprite lorsque Sonic n'est pas sur une plateforme 
	if (!G.Sonic.Stable) {
		if (G.Sonic.Super) {
			if (G.Sonic.Next_move.x < -0.1 && Moving()) idsprite = G.Sonic.SuperList[1];
			else if (G.Sonic.Next_move.x > 0.1 && Moving()) idsprite = G.Sonic.SuperList[2];
			else if (G.Sonic.Left && G.Sonic.Super) {
				idsprite = G.Sonic.SuperList[3];
			}
			else {
				idsprite = G.Sonic.SuperList[0];
			}
		}
		else {
			if (G.Sonic.Left) {
				idsprite = G.Sonic.JumpLeft[G.Sonic.JumpCounter];
			}
			else {
				idsprite = G.Sonic.JumpRight[G.Sonic.JumpCounter];
			}
		}
	}
	for (int i = 0; i < G.PowerUps.size(); i++) {
		if (G.PowerUps[i].Collected) {
			G2D::drawStringFontRoman(NewReferentiel(V2(G.PowerUps[i].Pos.x + 200, G.PowerUps[i].Pos.y + 200), decalage), "appuyez sur a pour attaquer", 50, 3, Color::Black);
		}
	}
	if (G.Eggman.health == 0) {
		G2D::drawRectangle(V2(0, 0), V2(G.WidthPix, G.HeighPix), Color::Blue, true);
		G2D::drawStringFontMono(V2(100, G.HeighPix / 2), string("VICTORY !!!"), 50, 5, Color::Yellow);
	}
	//Sprite lorsque Sonic est en train de charger son Kameha à gauche puis à droite
	if (G.Sonic.KamehaState && G.Sonic.Left) idsprite = G.Sonic.IdKamehaLeft;
	else if (G.Sonic.KamehaState && !G.Sonic.Left) idsprite = G.Sonic.IdKamehaRight;

	G2D::drawRectWithTexture(idsprite, NewReferentiel(G.Sonic.Pos, decalage), G.Sonic.Size);

	if(G.Eggman.health < 1) G2D::drawRectWithTexture(G.background.VictoryScreen, V2(0,0), V2(G.WidthPix, G.HeighPix));

	G2D::Show();
}




//gravité
void Gravity(GameData& G) {
	if (G.Sonic.Stable && !G2D::isKeyPressed(Key::UP)) G.Sonic.Next_move.y = 0;
	V2 gravity = V2(0, -0.35);
	if (G.Sonic.Pos.y >= 0 && !G.Sonic.Stable && !G.Sonic.KamehaState) {
		G.Sonic.Next_move = G.Sonic.Next_move + gravity;
	}
}

//Compteur animation de la background
void Animation(GameData& G) {
	if (!G.Sonic.FightAgainstBoss) {
		G.background.compteur = G.background.compteur + 0.5;
		if (G.background.compteur == 7) {
			G.background.compteur = 0;
		}
	}
	else {
		G.background.compteur = 0;
		G.background.compteur2 = G.background.compteur2 + 0.25;
		if (G.background.compteur2 == 1) G.background.IdFrame = 2;
		if (G.background.compteur2 == 2) G.background.IdFrame = 1;
		if (G.background.compteur2 == 3) G.background.IdFrame = 1;
		if (G.background.compteur2 == 4) G.background.IdFrame = 3;
		if (G.background.compteur2 == 5) G.background.IdFrame = 0;
		if (G.background.compteur2 == 6) G.background.IdFrame = 2;
		if (G.background.compteur2 == 7) G.background.IdFrame = 3;
		if (G.background.compteur2 == 8) G.background.IdFrame = 0;
		if (G.background.compteur2 == 7) G.background.IdFrame = 3;
		if (G.background.compteur2 == 8) G.background.IdFrame = 0;
		if (G.background.compteur2 == 7) G.background.IdFrame = 3;
		if (G.background.compteur2 == 8) G.background.IdFrame = 0;
		if (G.background.compteur2 == 9) G.background.compteur2 = 0;

	}
	for (int i = 10; i < G.Ennemis.size(); i++) {
		G.Ennemis[i].compteur = G.Ennemis[i].compteur + 0.25;
		if (G.Ennemis[i].compteur == 2) {
			G.Ennemis[i].compteur = 0;
		}
	}
}

//fonction pour détecter si le Character est sur une plateforme en vérifiant si les hitbox se chevauchent
bool IsOnPlateform(GameData& G) {
	Character prevision = G.Sonic;
	prevision.Pos = prevision.Pos + prevision.Next_move;

	for (int i = 0; i < G.Pf.size(); i++) {
		if (CollisionHitbox(G.Sonic.hitbox, G.Pf[i].hitbox) == 1) {
			return true;
		}
	}
	for (int i = 0; i < G.PfBoss.size(); i++) {
		if (CollisionHitbox(G.Sonic.hitbox, G.PfBoss[i].hitbox) == 1) {
			return true;
		}
	}
	return false;
}

void Friction(GameData& G) {
	if (G.Sonic.Stable && !(G2D::isKeyPressed(Key::RIGHT) || G2D::isKeyPressed(Key::LEFT))) {
		if (G.Sonic.Super) G.Sonic.Next_move.x = G.Sonic.Next_move.x * 0.1;
		else {
			G.Sonic.Next_move.x = G.Sonic.Next_move.x * 0.88;
			G.Sonic.compteur = G.Sonic.compteur * 0.88;
		}
	}
	if (!G.Sonic.Stable && !(G2D::isKeyPressed(Key::RIGHT) || G2D::isKeyPressed(Key::LEFT))) {
		if (G.Sonic.Super) G.Sonic.Next_move.x = G.Sonic.Next_move.x * 0.5;
		else G.Sonic.Next_move.x = G.Sonic.Next_move.x * 0.97;
	}
}

void FollowSonic(GameData& G) {
	V2 MovementSpeed = V2(0.1, 0);
	if (!G.Sonic.FightAgainstBoss) {
		for (int i = 10; i < G.Ennemis.size(); i++) {
			if (G.Sonic.Pos.x < G.Ennemis[i].Pos.x) G.Ennemis[i].Speed = G.Ennemis[i].Speed - MovementSpeed;
			else G.Ennemis[i].Speed = G.Ennemis[i].Speed + MovementSpeed;
		}
	}
	else {
		for (int i = 10; i < G.Ennemis.size(); i++) {
			G.Ennemis[i].Speed = V2(0, 0);
		}
	}
}

void BossApparition(GameData& G) {
	if (G.Sonic.Pos.x > 5000) {
		G.Eggman.Next_move.y = 5;
		G.Eggman.LeftHandNext_move.y = 5;
		G.Eggman.RightHandNext_move.y = 5;
		if (G.Eggman.Pos.y > 595) {
			G.Eggman.Next_move.y = 0;
			G.Eggman.LeftHandNext_move.y = 0;
			G.Eggman.RightHandNext_move.y = 0;
			G.Eggman.IsEnabled = true;
		}
		G.Sonic.FightAgainstBoss = true;
	}
}

void Camera(GameData& G) {
	//bouge la camera
	G.Sonic.Camera.y = G.Sonic.Pos.y - (G.HeighPix / 3.7);
	G.Sonic.Camera.x = G.Sonic.Pos.x - (G.WidthPix / 2);
}

/*
void BossHands(GameData& G) {
	//bouge les mains du boss
	G.Eggman.LeftHandPos = V2(G.Eggman.Pos.x - 270, G.Eggman.Pos.y - 300);
	G.Eggman.RightHandPos = V2(G.Eggman.Pos.x + 319, G.Eggman.Pos.y - 300);
}
*/

///////////////////////////////////////////////////////////////////////////////
//
//
//      Gestion de la logique du jeu - reçoit en paramètre les données du jeu par référence

/*
	La taille du Character doit changer afin d'actualiser sa nouvelle hitbox
*/

void Logic(GameData& G) // appelé 20 fois par seconde

{

	int timer = G2D::elapsedTimeFromStartSeconds();
	G.Sonic.Bouge();
	//Si le joueur appuie sur A, que Sonic est en version Super et qu'il n'a pas de temps de récupération sur son Kameha, il peut le charger
	if (G2D::isKeyPressed(Key::A) && G.Sonic.Super && !G.Sonic.IsOnCooldown) {
		//La fonction Kameha permet de signaler que Sonic rentre en état de Kameha (KamehaState) et qu'il ne peut plus bouger
		G.Sonic.Kameha(true);
		G.Projectiles[10].IsEnabled = true;
		G.Projectiles[10].hitbox.IsActivated = true;
		if (!G.Sonic.Left) {
			G.Projectiles[10].AddSize(V2(0.75, 0.75));
			G.Projectiles[10].AddPos(V2(0, -0.4));
			G.Projectiles[10].ShootedFromLeft = false;
		}
		else {
			G.Projectiles[10].AddSize(V2(0.75, 0.75));
			G.Projectiles[10].AddPos(V2(-0.75, -0.4));
			G.Projectiles[10].ShootedFromLeft = true;
		}
	}
	else {
		/*
		Si le missile est activé (à l'écran), il est tiré avec une vitesse horizontale de 30 selon la direction où vise Sonic
		Les capacités de Sonic sont mises en temps de récupération et il ne peut plus tirer
		*/
		if (G.Projectiles[10].IsEnabled) {
			if (!G.Projectiles[10].ShootedFromLeft) {
				G.Projectiles[10].IsShooted(V2(30, 0));
			}
			else {
				G.Projectiles[10].IsShooted(V2(-30, 0));
			}
			G.Sonic.SetIsOnCooldown(true);
		}

		/*
		Si le missile a disparu, actualisation de la position du prochain projectile à tirer selon la direction de Sonic
		La capacité n'est plus en temps de récupération
		*/
		else {
			G.Projectiles[10].hitbox.IsActivated = false;
			G.Sonic.SetIsOnCooldown(false);
			G.Projectiles[10].Speed = V2(0, 0);
			if (!G.Sonic.Left) G.Projectiles[10].SetPos(V2(G.Sonic.Pos.x + 60, G.Sonic.Pos.y + 35));
			else G.Projectiles[10].SetPos(V2(G.Sonic.Pos.x, G.Sonic.Pos.y + 35));
		}
		//Si le projectile dépasse l'écran, sa hitbox devient nulle et il est désactivé
		if (G.Projectiles[10].Pos.x < G.Sonic.Pos.x - 650 || G.Projectiles[10].Pos.x > G.Sonic.Pos.x + 650) {
			G.Projectiles[10].IsEnabled = false;
			G.Projectiles[10].Size = V2(0, 0);
		}
		//Sonic n'est plus en train de préparer son Kameha, il peut donc de nouveau bouger, KamehaState vaut donc false
		G.Sonic.Kameha(false);
	}
	if (G2D::isOnPause()) return;

	// Respawn si on tombe dans le vide
	if (G.Sonic.Pos.y < -50) {
		//position initial
		G.Sonic.Respawn();
	}



	//Détecte la collision de hitbox entre le personnage et les ennemis ou les projectiles
	for (int i = 0; i < G.Ennemis.size(); i++) {
		if (CollisionHitbox(G.Projectiles[10].hitbox, G.Ennemis[i].hitbox)) {
			if (G.Projectiles[10].IsEnabled) {
				G.Ennemis[i].Disable();
			}
		}
		if (CollisionHitbox(G.Sonic.hitbox, G.Ennemis[i].hitbox)) {
			G.Sonic.Respawn();
		}
		if (G.Ennemis[i].Size == V2(0, 0) && i < 10) G.Projectiles[i].Disable(true);
	}

	//Détecte la collision de hitbox entre Sonic et les projectiles provenant des ennemis
	for (int i = 0; i < G.Projectiles.size() - 1; i++) {
		if (G.Projectiles[i].FromEnnemy) {
			if (CollisionHitbox(G.Projectiles[i].hitbox, G.Projectiles[10].hitbox)) {
				if (G.Projectiles[10].IsEnabled) {
					G.Projectiles[i].IsEnabled = false;
					G.Projectiles[i].Disable(G.Projectiles[i].IsEnabled);
				}
			}
			if (CollisionHitbox(G.Sonic.hitbox, G.Projectiles[i].hitbox)) {
				G.Sonic.Respawn();
			}
		}
	}

	//Détecte la collision de hitbox entre Sonic et les PowerUp, empêche à Sonic d'en reprendre un tant qu'il en a un d'actif
	for (int i = 0; i < G.PowerUps.size(); i++) {
		if (CollisionHitbox(G.Sonic.hitbox, G.PowerUps[i].hitbox) && !G.PowerUps[i].Collected && !G.Sonic.Super) {
			G.Sonic.Super = true;
			G.PowerUps[i].Collected = true;
		}
	}
	if (G.Eggman.health < 1) {
		G.Eggman.Disable();
		G.Sonic.Size = V2(0, 0);
	}
	if ((CollisionHitbox(G.Sonic.hitbox, G.Eggman.BossHitbox)  || CollisionHitbox(G.Projectiles[10].hitbox, G.Eggman.BossHitbox)) && !G.Eggman.IsImmuned && G.Eggman.IsEnabled) {
		G.Eggman.health = G.Eggman.health - 1;
		G.Eggman.CantBeAttacked = 0.1;
		G.Sonic.Next_move = V2(-25,10);
	}
	if (((CollisionHitbox(G.Sonic.hitbox, G.Eggman.LeftHandHitbox) && G.Eggman.LeftHandHitbox.IsActivated)) || (CollisionHitbox(G.Sonic.hitbox, G.Eggman.RightHandHitbox) && G.Eggman.RightHandHitbox.IsActivated)){
		G.Sonic.Respawn();
	}
	if (G.Eggman.CantBeAttacked > 0) {
		G.Eggman.CantBeAttacked = G.Eggman.CantBeAttacked + 0.25;
		G.Eggman.IsImmuned = true;
		if (G.Eggman.CantBeAttacked > 50) {
			G.Eggman.CantBeAttacked = 0;
			G.Eggman.IsImmuned = false;
		}
	}

	if (G.Eggman.IsEnabled) {
		G.Eggman.Clap();
		if (G.Eggman.health < 3) G.PfBoss[2].FallingPlatform();
		if (G.Eggman.health < 2) G.PfBoss[3].FallingPlatform();

	}

	
	cout << G.Eggman.health;
	cout << "\n";
	


	// si sonic atterit sur un bumper
	for (int i = 0; i < G.Bp.size(); i++) {
		if (CollisionHitbox(G.Sonic.hitbox, G.Bp[i].hitbox)) {
			G.Sonic.Next_move.y = 20;
		}
	}

	// Lancement des projectiles provenant des ennemis, si ils sortent de l'écran, ils sont relancés
	for (int i = 0; i < G.Projectiles.size() - 1; i++) {
		if (G.Projectiles[i].FromEnnemy) {
			if (G.Projectiles[i].IsEnabled) {
				G.Projectiles[i].IsShooted(V2(-5, 0));
			}
			if (G.Projectiles[i].Pos.x < 0) {
				G.Projectiles[i].Pos.x = G.Projectiles[i].StartingPoint.x;
			}
		}
	}
	// Quand Sonic tape les murs
	for (int i = 0; i < G.Pf.size(); i++) {
		Hitbox prevision = Hitbox(G.Sonic.Pos + G.Sonic.Next_move, G.Sonic.Size + G.Sonic.Next_move);
		int size = G.Sonic.hitbox.D.y - G.Sonic.hitbox.Pos.y;
		if (CollisionHitbox(prevision, G.Pf[i].hitbox) == 2) {
			G.Sonic.Next_move = V2(0, 0);
		}
		if (CollisionHitbox(prevision, G.Pf[i].hitbox) == 3) {
			G.Sonic.Next_move = V2(0, 0);
		}
		if (CollisionHitbox(prevision, G.Pf[i].hitbox) == 4) {
			G.Sonic.Next_move = V2(0, 0);
		}
	}

	for (int i = 0; i < G.PfBoss.size(); i++) {
		Hitbox prevision = Hitbox(G.Sonic.Pos + G.Sonic.Next_move, G.Sonic.Size + G.Sonic.Next_move);
		int size = G.Sonic.hitbox.D.y - G.Sonic.hitbox.Pos.y;
		if (CollisionHitbox(prevision, G.PfBoss[i].hitbox) == 2) {
			G.Sonic.Next_move = V2(0, 0);
		}
		if (CollisionHitbox(prevision, G.PfBoss[i].hitbox) == 3) {
			G.Sonic.Next_move = V2(0, 0);
		}
		if (CollisionHitbox(prevision, G.PfBoss[i].hitbox) == 4) {
			G.Sonic.Next_move = V2(0, 0);
		}
	}

	for (int i = 0; i < G.Teleports.size(); i++) {
		Hitbox prevision = Hitbox(G.Sonic.Pos + G.Sonic.Next_move, G.Sonic.Size + G.Sonic.Next_move);
		if (CollisionHitbox(prevision, G.Teleports[i].hitbox) != 0) {
			G.Sonic.Pos = V2(9400, 450);
		}
	}
	FollowSonic(G);
	// gravité du jeu
	G.Sonic.Stable = IsOnPlateform(G);
	Gravity(G);
	Friction(G);
	Animation(G);
	Camera(G);
	BossApparition(G);


	//Actualisation de la position du Character et de sa hitbox
	G.Sonic.Pos = G.Sonic.Pos + G.Sonic.Next_move;
	G.Sonic.hitbox = Hitbox(G.Sonic.Pos, G.Sonic.Size);

	G.Eggman.Pos = G.Eggman.Pos + G.Eggman.Next_move;
	G.Eggman.BossHitbox = Hitbox(G.Eggman.Pos, G.Eggman.Size);
	G.Eggman.LeftHandPos = G.Eggman.LeftHandPos + G.Eggman.LeftHandNext_move;
	G.Eggman.LeftHandHitbox = Hitbox(G.Eggman.LeftHandPos, G.Eggman.LeftHandSize);
	G.Eggman.RightHandPos = G.Eggman.RightHandPos + G.Eggman.RightHandNext_move;
	G.Eggman.RightHandHitbox = Hitbox(G.Eggman.RightHandPos, G.Eggman.RightHandSize);

	//Actualisation de la hitbox des missiles
	for (int i = 0; i < G.Projectiles.size(); i++) {
		G.Projectiles[i].hitbox = Hitbox(G.Projectiles[i].Pos, G.Projectiles[i].Size);
	}
	for (int i = 0; i < G.PfBoss.size(); i++) {
		G.PfBoss[i].Pos = G.PfBoss[i].Pos + G.PfBoss[i].Next_move;
		G.PfBoss[i].hitbox = Hitbox(G.PfBoss[i].Pos, G.PfBoss[i].Size);
	}
	for (int i = 0; i < G.Ennemis.size(); i++) {
		G.Ennemis[i].Pos = G.Ennemis[i].Pos + G.Ennemis[i].Speed;
		G.Ennemis[i].hitbox = Hitbox(G.Ennemis[i].Pos, G.Ennemis[i].Size);
	}

	G.idFrame += 1;
}

//Initialisation des textures
void AssetsInit(GameData& G)
{
	// Size passé en ref et texture en param
	G.Sonic.InitTextures();
	G.background.InitTextures();
	G.Eggman.InitTextures();
	for (int i = 0; i < G.Trees.size(); i++) {
		G.Trees[i].InitTextures();
	}
	for (int i = 0; i < G.Ennemis.size(); i++) {
		G.Ennemis[i].InitTextures();
	}
	for (int i = 0; i < G.Projectiles.size(); i++) {
		G.Projectiles[i].InitTextures();
	}
	for (int i = 0; i < G.Pf.size(); i++) {
		G.Pf[i].InitTextures();
	}
	for (int i = 0; i < G.PfBoss.size(); i++) {
		G.PfBoss[i].InitTextures();
	}
	for (int i = 0; i < G.PowerUps.size(); i++) {
		G.PowerUps[i].InitTextures();
	}
	for (int i = 0; i < G.Bp.size(); i++) {
		G.Bp[i].InitTextures();
	}
	for (int i = 0; i < G.Teleports.size(); i++) {
		G.Teleports[i].InitTextures();
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//
//        Démarrage de l'application



int main(int argc, char* argv[])
{
	GameData G;   // instanciation de l'unique objet GameData qui sera passé aux fonctions render et logic

	// crée la fenêtre de l'application
	G2D::initWindow(V2(G.WidthPix, G.HeighPix), V2(20, 20), string("G2D DEMO"));

	// nombre de fois où la fonction Logic est appelée par seconde
	int callToLogicPerSec = 50;

	AssetsInit(G);
	// lance l'application en spécifiant les deux fonctions utilisées et l'instance de GameData
	G2D::Run(Logic, Render, G, callToLogicPerSec, true);

	// aucun code ici
}