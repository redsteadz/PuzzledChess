<p align="center">
    <h1 align="center">CHARSICHESS</h1>
</p>
<p align="center">
    <em><code>► Another Chess board for Puzzle solving !</code></em>

</p>


![Video](https://github.com/redsteadz/CharsiChess/assets/86804632/eb0474ec-7673-4557-a7a0-c5895ccbb65b)


<p align="center">
	<img src="https://img.shields.io/github/license/redsteadz/CharsiChess?style=default&logo=opensourceinitiative&logoColor=white&color=0080ff" alt="license">
	<img src="https://img.shields.io/github/last-commit/redsteadz/CharsiChess?style=default&logo=git&logoColor=white&color=0080ff" alt="last-commit">
	<img src="https://img.shields.io/github/languages/top/redsteadz/CharsiChess?style=default&color=0080ff" alt="repo-top-language">
	<img src="https://img.shields.io/github/languages/count/redsteadz/CharsiChess?style=default&color=0080ff" alt="repo-language-count">
<p>
<p align="center">
	<!-- default option, no dependency badges. -->
</p>

<br><!-- TABLE OF CONTENTS -->

<details>
  <summary>Table of Contents</summary><br>

- [Overview](#-overview)
- [Features](#-features)
- [Repository Structure](#-repository-structure)
- [Modules](#-modules)
- [Getting Started](#-getting-started)
  - [Installation](#-installation)
  - [Usage](#-usage)
  - [Tests](#-tests)
- [Project Roadmap](#-project-roadmap)
- [Contributing](#-contributing)
- [License](#-license)
- [Acknowledgments](#-acknowledgments)

</details>
<hr>

## Overview

<code>Welcome to the Chess Puzzle Master project! This innovative and visually appealing chess puzzle application was developed as a part of a second-semester project. The goal of Chess Puzzle Master is to offer chess enthusiasts a dynamic and engaging way to sharpen their chess skills by solving puzzles extracted directly from Lichess - one of the most popular chess platforms available today.
</code>
<code>Chess Puzzle Master stands out due to its sleek interface and the animated movements of the chess pieces, which add a delightful and interactive dimension to the puzzle-solving experience. Each piece animates smoothly across the board to its destination, and even the suggested position squares, hinting where a piece might best be moved, come alive with animation.</code>

---

## Features
- Puzzle Source: All puzzles are dynamically loaded from Lichess, ensuring a vast and varied collection that is continually updated with new challenges.
- Three Unique Modes:
- One Shot: Test your precision and foresight by solving each puzzle in just a single attempt. Perfect for those who love a challenge and want to test their accuracy under pressure.
- Timed: With a countdown of 5 minutes, see how many puzzles you can solve within the time limit. It’s a race against time that requires quick thinking and rapid decision-making.
- No Limit: For those looking for a more relaxed experience, this mode allows you to take your time with each puzzle, making it perfect for learning and practice without any stress.
- Animated Pieces and Suggested Positions: To enhance the user experience, all chess pieces and suggestion squares are fully animated, bringing the game to life and making your puzzle-solving journey even more enjoyable.

---

## Repository Structure

```sh
└── CharsiChess/
    ├── CMakeLists.txt
    ├── assets
    │   ├── GUI.png
    │   ├── bB.png
    │   ├── bK.png
    │   ├── bN.png
    │   ├── bP.png
    │   ├── bQ.png
    │   ├── bR.png
    │   ├── chess7
    │   ├── pixel_png
    │   ├── pixel_png.zip
    │   ├── randLine.cpp
    │   ├── sound
    │   ├── svgtopng.zip
    │   ├── wB.png
    │   ├── wK.png
    │   ├── wN.png
    │   ├── wP.png
    │   ├── wQ.png
    │   └── wR.png
    ├── compile_commands.json
    ├── headers
    │   ├── animate
    │   ├── animate.h
    │   ├── constant.h
    │   ├── database.h
    │   ├── gui.h
    │   └── sound.h
    └── main.cpp
```

---

## Modules

<details closed><summary>.</summary>

| File                                                                                                | Summary                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |
| --------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [CMakeLists.txt](https://github.com/redsteadz/CharsiChess/blob/master/CMakeLists.txt)               | Sets up the Chess project with CMake for building and debugging. Copies assets to the build directory. Uses Raylib for graphics. Declares the main.cpp file as the executable, linking with Raylib library.                                                                                                                                                                                                                                                                                                                                                                     |
| [compile_commands.json](https://github.com/redsteadz/CharsiChess/blob/master/compile_commands.json) | Enables compilation details for Chess project files, aiding in the build process by specifying commands and output locations.                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
| [main.cpp](https://github.com/redsteadz/CharsiChess/blob/master/main.cpp)                           | SummaryThis code file within the CharsiChess repository contributes to the graphical user interface (GUI) of the chess application. It efficiently manages the display of different chess piece images and handles user interactions with the game board. The critical features it provides include rendering the board, displaying various chess piece graphics based on game state, and enabling player moves through intuitive interface interactions. This code file plays a crucial role in ensuring a seamless and engaging user experience within the chess application. |

</details>

<details closed><summary>headers</summary>

| File                                                                                  | Summary                         |
| ------------------------------------------------------------------------------------- | ------------------------------- |
| [animate.h](https://github.com/redsteadz/CharsiChess/blob/master/headers/animate.h)   | <code> Features the animations played out in moving pieces, making squares and displaying some text </code> |
| [constant.h](https://github.com/redsteadz/CharsiChess/blob/master/headers/constant.h) | <code> Has all the global variables of the game </code> |
| [database.h](https://github.com/redsteadz/CharsiChess/blob/master/headers/database.h) | <code> Features the functions used to Access the hefty database of lichess </code> |
| [gui.h](https://github.com/redsteadz/CharsiChess/blob/master/headers/gui.h)           | <code> Makes some custom built UI interactables </code> |
| [sound.h](https://github.com/redsteadz/CharsiChess/blob/master/headers/sound.h)       | <code> Simply has the sounds in an easy to use map </code> |

</details>

---

## Getting Started

**System Requirements:**

- **C**: `version x.y.z`

### Installation

<h4>From <code>source</code></h4>

> 1. Clone the CharsiChess repository:
>
> ```console
> $ git clone https://github.com/redsteadz/CharsiChess
> ```
>
> 2. Change to the project directory:
>
> ```console
> $ cd CharsiChess
> ```
>
> 3. Make the build folder:
>
> ```console
> $ mkdir build
> $ cd build
> ```
> 4. Make the prerequisites for CMake:
>
> ```console
> $ cmakke ..
> ```
> 4. Build:
>
> ```console
> $ make
> ```
### Usage

<h4>From <code>source</code></h4>

> Run CharsiChess using the command below:
>
> ```console
> $ ./Chess
> ```

### Tests

> Run the test suite using the command below:
>
> ```console
> $ /* No common unit test framework in C */
> ```

---

## Project Roadmap

- [x] `► Make a menu`
- [ ] `► Finalize the conditions for chess and all it's variations in moves`
- [ ] `► Hook it up to a Chess engine (Make a chess engine ? )`
- [ ] `► ...`

---

## Contributing

Contributions are welcome! Here are several ways you can contribute:

- **[Report Issues](https://github.com/redsteadz/CharsiChess/issues)**: Submit
  bugs found or log feature requests for the `CharsiChess` project.
- **[Submit Pull Requests](https://github.com/redsteadz/CharsiChess/blob/main/CONTRIBUTING.md)**:
  Review open PRs, and submit your own PRs.
- **[Join the Discussions](https://github.com/redsteadz/CharsiChess/discussions)**:
  Share your insights, provide feedback, or ask questions.

<details closed>
<summary>Contributing Guidelines</summary>

1. **Fork the Repository**: Start by forking the project repository to your
   github account.
2. **Clone Locally**: Clone the forked repository to your local machine using a
   git client.
   ```sh
   git clone https://github.com/redsteadz/CharsiChess
   ```
3. **Create a New Branch**: Always work on a new branch, giving it a descriptive
   name.
   ```sh
   git checkout -b new-feature-x
   ```
4. **Make Your Changes**: Develop and test your changes locally.
5. **Commit Your Changes**: Commit with a clear message describing your updates.
   ```sh
   git commit -m 'Implemented new feature x.'
   ```
6. **Push to github**: Push the changes to your forked repository.
   ```sh
   git push origin new-feature-x
   ```
7. **Submit a Pull Request**: Create a PR against the original project
   repository. Clearly describe the changes and their motivations.
8. **Review**: Once your PR is reviewed and approved, it will be merged into the
   main branch. Congratulations on your contribution!

</details>

<details closed>
<summary>Contributor Graph</summary>
<br>
<p align="center">
   <a href="https://github.com{/redsteadz/CharsiChess/}graphs/contributors">
      <img src="https://contrib.rocks/image?repo=redsteadz/CharsiChess">
   </a>
</p>
</details>

---

## License

This project is protected under the
[SELECT-A-LICENSE](https://choosealicense.com/licenses) License. For more
details, refer to the [LICENSE](https://choosealicense.com/licenses/) file.

---

## Acknowledgments

- List any resources, contributors, inspiration, etc. here.

[**Return**](#-overview)

---
