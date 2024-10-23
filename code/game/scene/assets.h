#ifndef ASSETS_H
#define ASSETS_H

//actors
Actor player = actor_create(0, "rom:/game/pipo.t3dm");
ActorAnimation player_animation = actorAnimation_create(&player);
actorAnimation_init(&player, &player_animation);

//scenery
Scenery room = scenery_create(0, "rom:/game/testLevel.t3dm");
Scenery n64logo = scenery_create(0, "rom:/game/n64logo.t3dm");

#endif