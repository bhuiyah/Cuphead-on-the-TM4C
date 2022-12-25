// Cuphead.c
// Runs on TM4C123
// Sajid Bhuiyan and Calvin Heischman

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer1.h"
#include "Timer0.h"
#include "Timer2.h"
#include "Timer3.h"
#include "DAC.h"

#define PF2       (*((volatile uint32_t *)0x40025010))
	
uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm
uint8_t flag = 0;
uint8_t count = 0;
uint8_t cupflag = 0;
uint8_t enemyflag = 0;
uint8_t jumpflag = 0;
uint8_t shootflag = 0;
uint8_t pauseflag = 0;
uint8_t timer1Aflag = 0;
uint8_t edgepriorityflag = 0;
uint8_t timer2Aflag = 0;
int background = 0;
int health = 10;
int score = 0;
uint8_t fireflystatus = 1;
uint8_t carnivinestatus = 1;
int x = 10;
int y = 100;
int shootx;
int shooty = 85;
int deathanimx = 65;
int deathanimy = 128;
int carniviney = 110;
uint8_t carnivineindex = 0;
int fireflyx = 130;
int fireflyindex = 0;
uint8_t deathflag = 0;
uint8_t language;
uint8_t boss = 0;
uint8_t bossstatus = 1;
uint8_t bossindex = 0;
int bosshealth=20;
uint8_t done = 0;
int acornx = 90;
int acorny = 60;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts


typedef enum{dead, alive} status_t;
struct sprite{
	const uint16_t *image;
	status_t life; 
	int dx;
	int dy;
};

typedef struct sprite sprite_t;

struct background{
	const uint16_t *bg;
};

typedef struct background background_t;

 sprite_t CupRightRunning [9] = {
	{R1, alive, 10, 0},
	{R2, alive, 10, 0},
	{R3, alive, 10, 0},
	{R4, alive, 10, 0},
	{R5, alive, 10, 0},
	{R6, alive, 10, 0},
	{R7, alive, 10, 0},
	{R8, alive, 10, 0},
	{R9, alive, 10, 0},
};
 
 sprite_t CupLeftRunning[9] = {
	{L1, alive, -10, 0},
	{L2, alive, -10, 0},
	{L3, alive, -10, 0},
	{L4, alive, -10, 0},
	{L5, alive, -10, 0},
	{L6, alive, -10, 0},
	{L7, alive, -10, 0},
	{L8, alive, -10, 0},
	{L9, alive, -10, 0},
};
 
sprite_t JumpAnimation[8] = {
		{J1, alive, 5, -12},
		{J2, alive, 5, -12},
		{J3, alive, 5, -12},
		{J4, alive, 5, -12},
		{J1, alive, 5, 12},
		{J2, alive, 5, 12},
		{J3, alive, 5, 12},
		{J4, alive, 5, 12},
		
	}; 

const sprite_t CupStand [3] = {
	{Stand, alive, 0, 0},
	{Stand2, alive, 0, 0},
	{Stand3, alive, 0 , 0},
};

const background_t BGS [4] = {
	{BG},
	{BG},
	{BG},
};

sprite_t shootanim [1] = {
	{ShootAnim, alive, 10, 0},
};

sprite_t carnivine [32] = {
	{E1, alive, 0, -5},
	{E2, alive, 0, -5}, 
	{E3, alive, 0, -5}, 
	{E4, alive, 0, -5},
	{E5, alive, 0, -5},
	{E6, alive, 0, -5},
	{E7, alive, 0, -5},
	{E8, alive, 0, -5},
	{E1, alive, 0, -5},
	{E2, alive, 0, -5}, 
	{E3, alive, 0, -5}, 
	{E4, alive, 0, -5},
	{E5, alive, 0, -5},
	{E6, alive, 0, -5},
	{E7, alive, 0, -5},
	{E8, alive, 0, -5},
	{E8, alive, 0, 5},
	{E7, alive, 0, 5},
	{E6, alive, 0, 5},
	{E5, alive, 0, 5},
	{E4, alive, 0, 5},
	{E3, alive, 0, 5},
	{E2, alive, 0, 5},
	{E1, alive, 0, 5},
	{E8, alive, 0, 5},
	{E7, alive, 0, 5},
	{E6, alive, 0, 5},
	{E5, alive, 0, 5},
	{E4, alive, 0, 5},
	{E3, alive, 0, 5},
	{E2, alive, 0, 5},
	{E1, alive, 0, 5},
};

sprite_t firefly [16] = {
	{F1, alive, -10, 0},
	{F2, alive, -10, 0},
	{F3, alive, -10, 0},
	{F4, alive, -10, 0},
	{F1, alive, -10, 0},
	{F2, alive, -10, 0},
	{F3, alive, -10, 0},
	{F4, alive, -10, 0},
	{F1R, alive, 10, 0},
	{F2R, alive, 10, 0},
	{F3R, alive, 10, 0},
	{F4R, alive, 10, 0},
	{F1R, alive, 10, 0},
	{F2R, alive, 10, 0},
	{F3R, alive, 10, 0},
	{F4R, alive, 10, 0},

};

sprite_t EnemyDeathAnimation [8] = {
	{EnemyDeathAnim, alive, 0, 0},
	{EnemyDeathAnim, alive, 0, 0},
	{EnemyDeathAnim2, alive, 0, 0},
	{EnemyDeathAnim2, alive, 0, 0},
	{EnemyDeathAnim3, alive, 0, 0},
	{EnemyDeathAnim3, alive, 0, 0},
	{EnemyDeathAnim4, alive, 0, 0},
	{EnemyDeathAnim4, alive, 0, 0},

};

sprite_t DeathAnimation [32] = {
	{DeathAnim1, alive, 0, -4},
	{DeathAnim1, alive, 0, -4}, 
	{DeathAnim2, alive, 0, -4}, 
	{DeathAnim2, alive, 0, -4},
	{DeathAnim3, alive, 0, -4},
	{DeathAnim3, alive, 0, -4},
	{DeathAnim4, alive, 0, -4},
	{DeathAnim4, alive, 0, -4},
	{DeathAnim3, alive, 0, -4},
	{DeathAnim3, alive, 0, -4}, 
	{DeathAnim2, alive, 0, -4}, 
	{DeathAnim2, alive, 0, -4},
	{DeathAnim1, alive, 0, -4},
	{DeathAnim1, alive, 0, -4},
	{DeathAnim2, alive, 0, -4},
	{DeathAnim2, alive, 0, -4},
	{DeathAnim3, alive, 0, -4},
	{DeathAnim3, alive, 0, -4}, 
	{DeathAnim3, alive, 0, -4}, 
	{DeathAnim4, alive, 0, -4},
	{DeathAnim4, alive, 0, -4},
	{DeathAnim3, alive, 0, -4}, 
	{DeathAnim3, alive, 0, -4}, 
	{DeathAnim2, alive, 0, -4},
	{DeathAnim2, alive, 0, -4},
	{DeathAnim1, alive, 0, -4}, 
	{DeathAnim1, alive, 0, -4}, 
	{DeathAnim2, alive, 0, -4},
	{DeathAnim2, alive, 0, -4},
	{DeathAnim3, alive, 0, -4}, 
	{DeathAnim3, alive, 0, -4}, 
	{DeathAnim4, alive, 0, -4},
};

sprite_t Boss[30] = {
	{BossIdle1, alive, 0, 0},
	{BossIdle1, alive, 0, 0},
	{BossIdle2, alive, 0, 0}, 
	{BossIdle2, alive, 0, 0},
	{BossIdle3, alive, 0, 0},
	{BossIdle3, alive, 0, 0},
	{BossIdle1, alive, 0, 0},
	{BossIdle1, alive, 0, 0},
	{BossIdle2, alive, 0, 0}, 
	{BossIdle2, alive, 0, 0},
	{BossIdle3, alive, 0, 0},
	{BossIdle3, alive, 0, 0},
	{BossIdle1, alive, 0, 0},
	{BossIdle1, alive, 0, 0},
	{BossIdle2, alive, 0, 0}, 
	{BossIdle2, alive, 0, 0},
	{BossIdle3, alive, 0, 0},
	{BossIdle3, alive, 0, 0},
	{BossIdle1, alive, 0, 0},
	{BossIdle1, alive, 0, 0},
	{BossIdle2, alive, 0, 0}, 
	{BossIdle2, alive, 0, 0},
	{BossAttack1, alive, 0, 0},
	{BossAttack1, alive, 0, 0},
	{BossAttack2, alive, 0, 0},
	{BossAttack2, alive, 0, 0},
	{BossAttack3, alive, 0, 0},
	{BossAttack3, alive, 0, 0},
	{BossAttack4, alive, 0, 0},
	{BossAttack4, alive, 0, 0},	
};

void SysTick_Init(uint32_t period){
	NVIC_ST_CTRL_R = 0x00;
	NVIC_ST_RELOAD_R = period-1;	// reload value
	NVIC_ST_CTRL_R |=0x07;
}

void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	if(flag == 0){
		timer1Aflag = 1;			// carnivine is being set here when its the first background
	}
}

int moveLeft = 0;
int moveRight = 0;
int StandStill = 0;

void Timer0A_Handler(void){
  // write this
	Data = ADC_In();			//this is where movement occurs
	if(Data > 3500){
		moveRight = 1;			//right movement
	}
	else if(Data < 1000){ //left movement
		moveLeft = 1;   	
	}
	
	else if(Data >= 1000 && Data <= 3500){	//stand still
		StandStill = 1;
	}
	
	if(x > 160){					//go to the next stage
		flag++;
		x=0;
	}
	if(flag ==2){					//can't leave the boss fight
		if(x<0){
			x = 0;
		}
	}
		
	if(flag==1){					//you can go back from stage 2 to stage 1
		if(x < 0){
			flag--;
			x=150;
	}
}
	if(flag==0){					//stay on stage 0 if you want to move back
		if(x<0){
			x=0;
	}
}
	if(flag == 3){				//victory screen
		done = 1;
	}
	TIMER0_ICR_R = 0x01;
}

void Timer2AHandler(void){
	TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER2A timeout
	if(flag == 1){
		timer2Aflag = 1;			//firefly appears in stage 2
	}
}


volatile uint32_t FallingEdges = 0;

void EdgeCounter_Init(void){   
  SYSCTL_RCGCGPIO_R |= 0x00000010; //  activate clock for port E
  for(uint8_t i = 2; i > 0; i++) //wait for clock to stabilize
  GPIO_PORTE_DIR_R &= ~0x0F;    //     make PE0-3 input
  GPIO_PORTE_DEN_R |= 0x0F;     //     enable digital I/O on PE0-3
  GPIO_PORTE_PDR_R |= 0x0F;     //     enable weak pull-down on PE0-3
  GPIO_PORTE_IS_R &= ~0x0F;     //     PE0-3 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x0F;    //     PE0-3 is not both edges
  GPIO_PORTE_IEV_R |= 0x0F;    //      PE0-3 rising edge event
  GPIO_PORTE_ICR_R = 0x0F;      //     clear flag0-3
  GPIO_PORTE_IM_R |= 0x0F;      //     arm interrupt on PE0-3
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF0F)|0x00000020; // (g) priority 3
  NVIC_EN0_R = 0x00000010;      //     enable interrupt 4 in NVIC	
}

void ShootFunc(void){
			int j = 0;
			int g = 0;
			playsound(Laser);
			while(shootx < 160){
				if(g == 0){
				shootx = x+30;
				} 
				else{
				ST7735_DrawBitmap(0, 109 , BGS[flag].bg, 160, 110);					
				ST7735_DrawSprite(shootx, y-15, ShootAnim, 10, 4);			//shootanimation
				ST7735_DrawSprite(x, y, CupStand[j].image, 30, 33);			//cuphead will stand still when shooting animation occurs
				shootx+=shootanim[0].dx;
				j= (j+1) %3;
				if(timer1Aflag){
					if(carnivinestatus){
					ST7735_DrawSprite(100, carniviney, carnivine[carnivineindex].image, 35, 40);
					if(((shootx+10>=105)&&(shootx+10<=130)&& (shooty<=carniviney-20) && (shooty-4>=carniviney-40)) ||((shootx+10>=115)&&(shootx+10<=125) && (shooty<=carniviney) && (shooty-4>=carniviney-20))){
						for(int i = 0; i<8; i++){
							ST7735_DrawSprite(100, carniviney, EnemyDeathAnimation[i].image, 32, 30); 			//collision between projectile and carnivine
							shootx = 161;
							DelayMs(25);
							}
						score+=50;
						carnivinestatus = 0;
						carniviney = -1;
						shootflag = 0;
					}
					carniviney+=carnivine[carnivineindex].dy;
					carnivineindex = (carnivineindex +1)%32;
					timer1Aflag = 0;
				}
			}
				if(timer2Aflag){
					if(fireflystatus){
					ST7735_DrawSprite(fireflyx, 100, firefly[fireflyindex].image, 25, 35); 
					fireflyx+=firefly[fireflyindex].dx;
					fireflyindex = (fireflyindex +1)%16;
					timer2Aflag = 0;
					if(((shootx>= fireflyx))&&(shootx<=fireflyx+25)){
						for(int i = 0; i<8; i++){
						ST7735_DrawSprite(fireflyx-4, 100, EnemyDeathAnimation[i].image, 32, 30);			//collision between projectile and firefly
						shootx = 161;
						DelayMs(25);
						}
						score+=20;
						fireflystatus = 0;
						fireflyx = -1;
						shootflag = 0;
					}
				}
			}
				if(boss){
					if(bossstatus){
						ST7735_DrawSprite(90, 100, Boss[bossindex].image, 60, 80);
						bossindex = (bossindex + 1)%30;
						boss = 0;
						if((shootx>=115)&&(shootx<=160)){
							bosshealth = bosshealth -5;
							shootx = 161;
							shootflag = 0;
						}
						if(bosshealth == 0){
							for(int i = 0; i<8; i++){
								ST7735_DrawSprite(100, 100, EnemyDeathAnimation[i].image, 32, 30);
								shootx = 161;
								DelayMs(25);
							}
							score+=100;
							bossstatus = 0;
							shootflag = 0;
						}
					}
				}					
			}
			g++;
		}
			shootx = x+30;
	}

void JumpFunc(void){
	int jumpdelay = 30000;
	playsound(Jump);
		for(int i =0; i<8; i++){
			while(jumpdelay != 0){jumpdelay--;}
			ST7735_DrawBitmap(0, 109 , BGS[flag].bg, 160, 110);
			ST7735_DrawSprite(x, y, JumpAnimation[i].image, 30, 33);
			x+=JumpAnimation[i].dx;
			y+=JumpAnimation[i].dy;
			if(timer1Aflag){
				if(carnivinestatus){
				ST7735_DrawSprite(100, carniviney, carnivine[carnivineindex].image, 35, 40);
				carniviney+=carnivine[carnivineindex].dy;
				carnivineindex = (carnivineindex +1)%32;
				timer1Aflag = 0;
				}
			}
			if(timer2Aflag){
				if(fireflystatus){
				ST7735_DrawSprite(fireflyx, 100, firefly[fireflyindex].image, 25, 35); //cuphead right animation
				fireflyx+=firefly[fireflyindex].dx;
				fireflyindex = (fireflyindex +1)%16;
				timer2Aflag = 0;
				}
			}
			if(boss){
				if(bossstatus){
					ST7735_DrawSprite(90, 100, Boss[bossindex].image, 60, 80);
					bossindex = (bossindex + 1)%30;
					boss = 0;
			}
		}
			jumpdelay = 30000;
		}
	}

void PauseFunc(void){
	DisableInterrupts();
	ST7735_FillScreen(0);
	ST7735_SetCursor(11, 5);
	if(language == 1){
		ST7735_OutString("PAUSE ");
		ST7735_SetCursor(7, 7);
		ST7735_OutString("Green to Play ");
	}
	if (language == 0){
		ST7735_OutString("Détente ");
		ST7735_SetCursor(7, 7);
		ST7735_OutString("Vert Pour Jouer ");
	}
	DelayMs(300);
	while(GPIO_PORTE_DATA_R != 0x01){};
	DelayMs(300);
	EnableInterrupts();
}

void DoneFunc(void){
	DisableInterrupts();
	ST7735_FillScreen(0);
	ST7735_SetCursor(10, 5);
	if(language == 1){
		ST7735_OutString("You Win! ");
		ST7735_SetCursor(7, 7);
		ST7735_OutString("Your Score:");
		ST7735_OutUDec(score);
	}
	if(language == 0){
		ST7735_OutString("Vous Gagnez ");
		ST7735_SetCursor(7, 7);
		ST7735_OutString("Vos Ames: ");
		ST7735_OutUDec(score);
	}
	while(1){};
}
			

void GPIOPortE_Handler(void){
	if((GPIO_PORTE_DATA_R & 0x04)>>2 == 1){
		shootflag = 1;
		edgepriorityflag = 1;
	}
	if((GPIO_PORTE_DATA_R & 0x02)>>1 == 1){
		jumpflag = 1;
		edgepriorityflag = 1;
	}
	if((GPIO_PORTE_DATA_R & 0x01) == 1){
		pauseflag = 1;
		edgepriorityflag = 1;
	}
  GPIO_PORTE_ICR_R = 0x07;      // acknowledge flag4
  FallingEdges = FallingEdges + 1;
	
}

void Death(void){
	DisableInterrupts();
	while(1){
		int i;
		for(i=0; i<32; i++){
			ST7735_FillScreen(0x001F);
			ST7735_DrawSprite(deathanimx, deathanimy, DeathAnimation[i].image, 30, 33);
			deathanimy+=DeathAnimation[i].dy;
		}
		i=0;
		deathanimy = 128;
	}
}

void SysTick_Handler(void){
	if(deathflag){
		Death();
	}
	if(flag == 2){
		boss = 1;
	}
	background = 1;
}

int main(void){
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
  Output_Init();
	ST7735_InvertDisplay(1);
	ADC_Init();
	IO_Init();
	ST7735_SetRotation(1);
	EnableInterrupts();
	ST7735_DrawBitmap(0, 127 , TitleScreen, 160, 128);
	language = IO_Touch();
	ST7735_FillScreen(0);
	EdgeCounter_Init();
	Timer0_Init(4000, 2);
	Timer1_Init(4000, 3);
	Timer2_Init(&Timer2AHandler, 4000);
	SysTick_Init(2000);
	DAC_Init();
	EnableInterrupts();
	int j = 0;
	int g = 0;
	int k = 0;
  while(1){
		if(done == 1){
			DoneFunc();
		}
		if(health == 0){
				deathflag =1;
		}
		if(background == 1){
			ST7735_DrawBitmap(0, 109 , BGS[flag].bg, 160, 110);
			background = 0;
		}
		ST7735_SetCursor(0,11);
		if(language == 1){
			ST7735_OutString("Health:");
		}
		if(language == 0){
			ST7735_OutString("Sante:");
		}
		ST7735_OutUDec(health);
		if(health == 9){
			ST7735_OutChar(0x00);
		}
		ST7735_SetCursor(0,12);
		if(language == 1){
			ST7735_OutString("Score:");
		}
		if(language == 0){
			ST7735_OutString("Ames:");
		}
		ST7735_OutUDec(score);
		if(flag == 2){
			ST7735_SetCursor(13, 11);
			ST7735_OutString("Cagney:");
			ST7735_OutUDec(bosshealth);
			if(bosshealth < 10){
				ST7735_OutChar(0x00);
			}
		}
		if(timer1Aflag){
			if(carnivinestatus){
			ST7735_DrawSprite(100, carniviney, carnivine[carnivineindex].image, 35, 40); //cuphead right animation
			carniviney+=carnivine[carnivineindex].dy;
			carnivineindex = (carnivineindex +1)%32;
			timer1Aflag = 0;
			if((x+30>=100)&&(x+30<=135)&& (y<=carniviney) && (y-33>=carniviney-40)){
					health--;
			}
		}
	}
		if(timer2Aflag){
			if(fireflystatus){
				ST7735_DrawSprite(fireflyx, 100, firefly[fireflyindex].image, 25, 35); //cuphead right animation
				fireflyx+=firefly[fireflyindex].dx;
				fireflyindex = (fireflyindex +1)%16;
				timer2Aflag = 0;
				if(((x+30>= fireflyx))&&(x+30<=fireflyx+25)){
					health--;
			}
		}
	}
		if(boss){
			if(bossstatus){
				ST7735_DrawSprite(90, 100, Boss[bossindex].image, 60, 80);
				bossindex = (bossindex + 1)%30;
				boss = 0;
			}
		}
		
		if(edgepriorityflag){
			if(shootflag){
				ShootFunc();
				shootflag = 0;
				}
			if(jumpflag){
				JumpFunc();
				jumpflag = 0;
			}	
			if(pauseflag){
				PauseFunc();
				pauseflag = 0;
			}
			edgepriorityflag = 0;
		}
		if(!edgepriorityflag){
			if(moveRight){
				ST7735_DrawSprite(x, y, CupRightRunning[j].image, 30, 33); //cuphead right animation
				x+=CupRightRunning[j].dx;
				j=(j+1)%9;
				moveRight = 0;
		}
			if(moveLeft){
				ST7735_DrawSprite(x, y, CupLeftRunning[g].image, 30, 33); //cuphead right animation
				x+=CupLeftRunning[g].dx;
				g=(g+1)%9;
				moveLeft = 0;
		}
			if(StandStill){
				ST7735_DrawSprite(x, y, CupStand[k].image, 30, 33); //cuphead right animation
				k=(k+1)%3;
				StandStill = 0;
		}
	}
}
	}
