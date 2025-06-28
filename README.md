# Red Door Run

## Section 1: Summary
Team members: Andrea Li, Arjun Sharma, Amudhan Gurumoorthy

Concept statement: Experience what it’s like to make a desperate dash for quesadillas in the infinite single-player experience of _Red Door Run_. Try to keep your sprite alive as long as possible, and collect as many quesadillas as you can, while jumping to avoid the tables that spawn in your way.

## Section 2: Gameplay
Red Door Run is a single-player game utilizing collisions, quesadilla collection, and a leaderboard. The main objective of the game is to run as long as possible, and to collect quesadillas on the way. Users can use quesadillas to upgrade their attire and add lives. After a certain time, the speed of the sprite will increase, and will continue until the sprite crashes into a table. Once they have collided with a table, and the number of lives runs out, the game ends and restarts automatically.

The longest time is kept at the top of the screen along with the time of the current run. This acts like a “best time” to entice the user to play again. The player “wins” if they succeed to outrun their best time, and they “lose” if they collide and the game ends.

The space bar will allow the user to jump over the tables and avoid collision. We will use sprites to design the obstacles (tables of Red Door), the background, and the player itself which will all be custom.

Jumping up and falling down from the obstacles (tables) is where physics comes into play, as well as the change in velocity to increase the difficulty of the game after a certain amount of time has passed.

## Section 3: Feature Set
Here are all of the individual features we will have in our game:

1. Basic Mechanics: Moving our character on screen by adding a forward velocity to the particle and having the character jump (increasing the y-velocity) whenever the spacebar key is ptwressed and duck (decreasing the height of the player)  
2. Obstacle spawning: Randomly spawning obstacles within the screen, each of an arbitrary size. We will need to incorporate kinematics-based estimations to guarantee adequate spacing between consecutive obstacles, augmented with some additional random spacing. This will depend on the current velocity / difficulty of the game.
3. Obstacle collision handling: Head-on collisions with obstacles should result in the player losing; but landing on top of the obstacle should affect the kinematic state of the player by allowing them to 'run' on top of the obstacle and fall off once they hit the edge. This will require nontrivial modifications to typical collision handling logic due to the requirement to continuously check the state of the collision.
4. Camera tracking logic: The player sprite must be centred at the middle of the screen, so an internal camera state must be managed which centres the viewport on the player, taking into account their current position.  
5. Continuously rendering the location: Given the camera state, we will need to render the correct main scene and background, with each layer moving at the appropriate speed with 2-D parallax.   
6. Leaderboard: The time the player survived in each round will be tracked and displayed at the top of the screen, along with their lifetime record.  
7. Custom Sprites: We will turn our obstacles and character into custom sprites that will add to the flashiness of the program  
8. Custom music and custom sound effects: we will find and set up an audio file asset to play in the background while the game is in progress. It will be started at the beginning of a new attempt and stopped when the player dies. Additionally, when the player collides with the obstacle, as well as when they jump, an audio file containing a sound will play during that moment  
9. Difficulty scaling: As the player progresses further and further into one particular run, the game will become harder, increasing the velocity of the player and increasing the rate of obstacles, while still making the game possible  
10. Generation and tracking of cheese quesadillas: Cheese quesadillas will randomly spawn at a controllable frequency in the path of the player. A player can collect quesadillas by colliding with them. The number of lifetime quesadillas collected will be tracked and displayed at the top of the screen.  
11. Shop: When the player dies, a shop will display, where the player can use their cheese quesadillas to buy new sprites and backgrounds that they can add to the game.   
12. Magnet powerup: At rare random intervals in the game, a magnet sprite will appear in the path of the player. After the player collides with the magnet, a magnet powerup will be activated for a limited amount of time. During this period, coins will accelerate from in front of the player towards the player and be collected automatically.
