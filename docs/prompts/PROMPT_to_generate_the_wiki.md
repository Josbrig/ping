
You are working inside a GitHub project with two repositories:
1. The main repository "ping"
2. The GitHub Wiki repository "ping.wiki"

Both repositories are part of the same VS Code workspace.  
Your task is to create a complete, well‑structured GitHub Wiki for the project "Ping".

=== PROJECT CONTEXT ===
- The entire project "Ping" was created exclusively through Prompt‑Engineering.
- All code was generated using the Roo plugin together with GPT‑5.1.
- The project is an experiment in AI‑driven development.
- The documentation should reflect this origin clearly.
- The Wiki must be written in GitHub‑compatible Markdown.
- The Wiki must be fully self‑contained and internally linked.
- The Wiki must support embedding of Doxygen‑generated diagrams (images will be placed later in /images).

=== YOUR TASK ===
Create a complete GitHub Wiki consisting of multiple Markdown pages.  
Generate the full content for each page.

=== WIKI STRUCTURE TO GENERATE ===
1. Home.md  
   - Overview of the project  
   - Purpose of the repository  
   - Explanation that the entire project was created using Prompt‑Engineering  
   - Link to all other Wiki pages  

2. Architecture.md  
   - High‑level architecture description  
   - Module overview  
   - Component responsibilities  
   - Placeholders for Doxygen diagrams (use Markdown image syntax)  

3. Prompt-Engineering.md  
   - Explanation of how the project was created  
   - Description of the workflow with Roo and GPT‑5.1  
   - Examples of prompts used  
   - Lessons learned  

4. Development-Workflow.md  
   - How development is performed using prompts  
   - Branching strategy  
   - How to use Roo in this project  
   - How to regenerate or extend code  

5. API.md  
   - Overview of public interfaces  
   - Function descriptions  
   - Links to Doxygen documentation (relative links to /docs/index.html)  

6. Doxygen-Integration.md  
   - How Doxygen is configured  
   - How diagrams are generated  
   - How to embed diagrams in the Wiki  
   - Markdown examples for embedding images  

7. Roadmap.md  
   - Planned features  
   - Future improvements  
   - Ideas for extending the Prompt‑Engineering approach  

=== REQUIREMENTS ===
- Use clean, well‑structured Markdown.
- Include headings, lists, tables, and code blocks where appropriate.
- Add cross‑links between all pages using relative Wiki links:
  [[Home]] [[Architecture]] [[Prompt-Engineering]] etc.
- Add placeholders for diagrams like:
  ![Class Diagram](images/class_diagram.png)
- Write in clear, technical English.
- Do NOT generate HTML.
- Do NOT generate binary files.
- Only generate Markdown content.

=== OUTPUT FORMAT ===
Produce the full content for all pages in one response, separated like this:

--- FILE: Home.md ---
<content>

--- FILE: Architecture.md ---
<content>

--- FILE: Prompt-Engineering.md ---
<content>

...and so on for all pages.

Begin now.
