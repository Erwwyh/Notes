## 引言
hello everyone, the paper i introduce is an introduction to rocker: docker container for R

## 介绍

the paper is an introduction for a new tech, which is called rocker. this is a new tool for solving some problems such as the untouchable container compose, unmoveable imges on the docker container, a virtual technology build for apart sessions from each other. and then help math area researchers develop the environment of R language.

## Design principles & use cases

### 与docker

the introduction of rocker lead us come to is main fock, docker. it's has some interestion and useful features.

the first is 

**Isolation**, Docker containers provide process and filesystem isolation, allowing each container to run independently and avoid dependency conflicts and environment pollution.

**Moveable**: Docker images can run on any platform that supports Docker, including local development machines, servers, and cloud services. This ensures you can deploy and run the same R environment anywhere.

**Replayable**: Using Docker images, developers can ensure consistency across development, testing, and production environments, eliminating the "it works on my machine" problem.

**Version Control**: Docker images can be tagged to denote specific versions. Rocker leverages this to provide different versions of R environments, enabling users to easily switch or specify the required version.

**High Resource Efficiency**: Docker containers share the host operating system's kernel, making them more lightweight and efficient than traditional virtual machines.

**Automated Builds**: Docker provides Dockerfiles to define the image-building process. The Rocker Project uses Dockerfiles to create and maintain its pre-configured R images.

### 自身特性

Roocker inherite these features from docker, and make its own features. particularly beneficial for R users looking to leverage containerization.

**Pre-configured Images**: Provides a variety of pre-configured Docker images tailored for different R environments, including base R, RStudio Server, Shiny Server, and more.

**Extensibility**: Rocker images can be customized and extended as needed, allowing users to install additional packages or make configuration adjustments on top of the base images.

**Cross-platform Support**: Rocker images can run on various platforms, including local development setups, cloud computing services (like AWS and Google Cloud), and high-performance computing clusters.

**Community Support**: The Rocker Project is community-driven, with active participation from users and developers, providing extensive documentation and support resources.

## Rocker organization and workflow

image：base on Debain and Debain Stable

image build



## 引用文献

