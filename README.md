# Red Door Run
#### Caltech CS3 SP25 Game Design Document

## Section 0: Summary
Game title: _Red Door Run_

Team members: Amudhan Gurumoorthy, Andrea Li, Arjun Sharma

Concept statement: Experience what it’s like to make a desperate dash for quesadillas in the infinite single-player experience of _Red Door Run_. Try to keep your sprite alive as long as possible, and collect as many quesadillas as you can, while jumping to avoid the tables that spawn in your way.

## Section 1: Gameplay
Red Door Run is a single-player game utilizing collisions, quesadilla collection, and a leaderboard. The main objective of the game is to run as long as possible, and to collect quesadillas on the way. Users can use quesadillas to upgrade their attire and add lives. After a certain time, the speed of the sprite will increase, and will continue until the sprite crashes into a table. Once they have collided with a table, and the number of lives runs out, the game ends and restarts automatically.

The longest time is kept at the top of the screen along with the time of the current run. This acts like a “best time” to entice the user to play again. The player “wins” if they succeed to outrun their best time, and they “lose” if they collide and the game ends.

The space bar will allow the user to jump over the tables and avoid collision. We will use sprites to design the obstacles (tables of Red Door), the background, and the player itself which will all be custom.

Jumping up and falling down from the obstacles (tables) is where physics comes into play, as well as the change in velocity to increase the difficulty of the game after a certain amount of time has passed.

## Section 2: Feature Set
Here are all of the individual features we will have in our game:

Here are all of the individual features we will have in our game:

1. Basic Mechanics (Priority 1): Moving our character on screen by adding a forward velocity to the particle and having the character jump (increasing the y-velocity) whenever the spacebar key is ptwressed and duck (decreasing the height of the player)  
2. Obstacles (Priority 1): Randomly spawning obstacles within the screen, each with an arbitrary size and having a random amount of space between obstacles. These obstacles will currently be rectangles with a height and a random width (causing the player to jump) and an elevated rectangle (causing the player to duck).  
3. Camera tracking logic (Priority 1): The player sprite must be centred at the middle of the screen, so an internal camera state must be managed which centres the viewport on the player, taking into account their current position.  
4. Continuously rendering the location (Priority 1): Given the camera state, we will need to render the correct main scene and background, with each layer moving at the appropriate speed with 2-D parallax.   
5. Leaderboard (Priority 2): The time the player survived in each round will be tracked and displayed at the top of the screen, along with their lifetime record.  
6. Custom Sprites (Priority 2): We will turn our obstacles and character into custom sprites that will add to the flashiness of the program  
7. Custom music and custom sound effects (Priority 2): we will find and set up an audio file asset to play in the background while the game is in progress. It will be started at the beginning of a new attempt and stopped when the player dies. Additionally, when the player collides with the obstacle, as well as when they jump, an audio file containing a sound will play during that moment  
8. Difficulty scaling (Priority 3): As the player progresses further and further into one particular run, the game will become harder, increasing the velocity of the player and increasing the rate of obstacles, while still making the game possible  
9. Generation and tracking of cheese quesadillas (Priority 3): Cheese quesadillas will randomly spawn at a controllable frequency in the path of the player. A player can collect quesadillas by colliding with them. The number of lifetime quesadillas collected will be tracked and displayed at the top of the screen.  
10. Shop (Priority 4): When the player dies, a shop will display, where the player can use their cheese quesadillas to buy new sprites and backgrounds that they can add to the game.  
11. Revival powerup (Priority 4): Before a game, players will be able to buy a powerup using their quesadillas that will allow them to revive after their first collision in the next game.  
12. Magnet powerup (Priority 4): At rare random intervals in the game, a magnet sprite will appear in the path of the player. After the player collides with the magnet, a magnet powerup will be activated for a limited amount of time. During this period, coins will accelerate from in front of the player towards the player and be collected automatically.

## Section 3: Timeline

**Week 1:**

1. (Amudhan) Basic mechanics   
2. (Andrea) Obstacles spawning  
3. (Arjun) Camera tracking logic  
4. (Arjun) Rendering the background

**Week 2:**

1. (Amudhan) Leaderboard  
2. (Andrea) Custom sprites  
3. (Andrea) Custom music and custom sound effects   
4. (Amudhan) Difficulty scaler  
5. (Arjun) Generation and tracking of cheese quesadillas in game

**Week 3:**

1. (Andrea) Shop  
2. (Amudhan) Revival powerup  
3. (Arjun) Magnet powerup

## Section 4: Disaster Recovery
Amudhan: If I fall behind, I will set a stronger priority for this CS game over my other classes, fully focusing my effort on making sure I am caught up and taking any extensions that I am able to in other classes. I will also communicate with my teammates about any problems I have or whether I am falling behind, so everyone will be on the same page. Then, if I feel like I am really falling behind on one particular problem, and my teammates do not know how to fix it, I will go to office hours and ask for any help that I need. This will help me continue my flow of work and start working on the next steps of my project.

Andrea: I will first communicate with my teammates to let them know the situation and also our current status of the game. It would be necessary to discuss what happens next, like perhaps pushing back our schedule and adjusting our timeline. I should keep track of my own assignments and the next steps I should take to get myself caught up. This may mean asking for extensions of other classes to prioritize this project. I want to make sure that I am completely transparent with my groupmates and that I am actively making sure that I do get caught up, like setting alarms, setting to-do lists, and going to OH to keep myself accountable.

Arjun: Throughout the project, I will try my best to be in frequent communication with my teammates, keeping them up to date with the number and scale of any significant issues I might face. I will try to be well-prepared when asking for help in office hours to make sure I am maximally efficient in debugging issues as they come up, to try to avoid being bogged down by individual bugs. If I fall behind, I will try my best to take out a large dedicated time block to catch up, asking for help from my teammates if they are available. If necessary, I should be able to make use of my extensions for my other classes, all of which I still have available as of week 7. I expect this class to be my primary time commitment for the remainder of the term and will structure my schedule accordingly.
