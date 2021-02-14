# Apuestas
 
 Módulo de AzerothCore que permite apostar por medio de un NPC oro en el juego

##  Módulo de Azerothcore

* Configurable el oro como apuesta mínima para cada apostador.
* Retirable el premio vía NPC o Onlogin.
* Reune el oro de todos los apostadores y escoge un ganador al azar entre ellos para llevarse el premio.
* Entrega de premio de bolsa total ganada sólo por medio de un GM.

> Premio=(apuestaMin * CantidadParticipantes)

> conf\Apostador.conf

```
[worldserver]

#
# Valor del precio de la apuesta mínima en valor de cobre
# 
#

Cost=500000
```

## Requisitos

* v2.0.0+ [Azerothcore](https://github.com/azerothcore/azerothcore-wotlk) 

## Instalación 

```
1) Usar el comando `git clone` o descargarlo manualmente.
2) Importar el SQL manualmente en la base de datos Characters y World respectivamente.
3) Mover el contenido a la carpeta raíz de AzerothCore en /modules.
4) Re-run cmake y compilar AzerothCore limpio.
```

## Creditos 

* [Kiters](https://warsong.cl/) (Idea Principal)
* [Xhaher](https://github.com/xhaher) (Desarrollador del módulo)
* AzerothCore: [repository](https://github.com/azerothcore) - [website](http://azerothcore.org/) - [discord chat community](https://discord.gg/PaqQRkd)