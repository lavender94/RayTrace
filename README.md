# RayTrace
This is a ray tracing project written by C++ and without the use of OpenGL

# folder description
- please go to src/ to find all the scource code. To run the project, use vs2010 to bulid files under project/

# Tips for review
demo results can be found under res/, including several pictures and 2 short movies
* for code skill purpose: take a brief view of Object.h and its inherit classes (such as Sphere, Triangle) should be enough
* for project achitecture

  > this project is mainly made up of four parts:
    - Camera:   Definition of camera, includes position, direction, etc.
    - Scene:    Definition of both the whole scene of a subscene
    - Raytrace: Class for running ray tracing algorithm
    - Movie:    Class for generating movies
    
  > recommended review order is main.cpp, class Camera, class Scene, class Raytrace, class Movie
