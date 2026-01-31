# **AGENTEN_RULES.md – Behavioral Rules for AI Agents in this Project**  
*(Modern, minimalist, robust, AI-friendly)*

The project language is English. All texts in other languages should be replaced by English.

## **0. Purpose of this Document**
This document defines **all rules** by which an AI agent operates in this project.  
It ensures that **every AI agent**:

- processes tasks consistently  
- documents results completely  
- maintains TODO lists correctly  
- stores files in the right locations  
- works in a consistent, reproducible, and auditable manner  

These rules apply **to every prompt**, **every response**, and **every action** of the AI agent.

---

# **1. Core Principles of the AI Agent**
1. **Everything the agent receives or produces is archived.**  
2. **The agent works transparently, reproducibly, and traceably.**  
3. **The agent never overwrites files without explicit instruction.**  
4. **The agent asks questions when information is missing.**  
5. **The agent strictly follows the project structure.**  
6. **The agent works step by step and marks completed tasks autonomously.**

---

# **2. Project Structure (Directory Rules)**

The AI agent follows this fixed structure:

| Area | Purpose | Path |
|--------|-------|------|
| **Prompts** | Archive of all prompts, questions, answers | `docs/prompts/` |
| **Reports** | Result reports, work logs | `docs/reports/` |
| **TODO Files** | Task lists for agents | `docs/todo/` |
| **Concepts** | Architecture ideas, drafts, considerations | `docs/concepts/` |
| **Documentation** | Technical documentation for developers | `docs/documentation/` |
| **User Manuals** | Guides for people using the project | `docs/manuals/` |
| **Project Scripts** | Scripts that are part of the project | `scripts/project/` |
| **User Scripts** | Scripts that simplify usage | `scripts/user/` |

The agent **always** stores files in these areas.  
If an area is missing, the agent creates it.

---

# **3. Prompt Archiving**

For **every prompt**, including follow-up questions and answers, the agent creates an archive file:

- Location: `docs/prompts/`
- Format: Markdown
- Filename:  
  `Prompt_YYYYMMDD_HHMMSS_ShortName.md`

Contents:

1. Original prompt  
2. Agent questions  
3. User answers  
4. Final result  
5. References to generated files

---

# **4. Reports**

The agent creates a report when:

- a task has been completed  
- a file was created  
- a TODO list was updated  
- a concept was created  
- a problem occurred  

Rules:

- Location: `docs/reports/`
- Format: Markdown
- Filename:  
  `Report_YYYYMMDD_HHMMSS_Name.md`
- Contents:
  - Purpose  
  - Steps  
  - Results  
  - Created files  
  - Updated TODO items  

---

# **5. TODO Rules**

## **5.1 Structure of a TODO File**
A TODO file consists of:

- **Main items** (numbered: 1, 2, 3, …)
- **Sub-items** (A, B, C, …)
- **Sub-sub-items** (I, II, III, …)

Example:

```
1. [ ] Main task
   A. [ ] Subtask
      I. [ ] Sub-subtask
```

## **5.2 Rules for TODO Entries**
- Every item has a status field:
  - `[ ]` open  
  - `[x]` done  
- The agent **may add new items** but **must never delete** existing ones.  
- The agent marks items **autonomously as done** when:
  - the task is fully completed  
  - all sub-items are done  
  - the results have been documented  

## **5.3 Autonomous Processing**
The agent processes TODO lists as follows:

1. **Read the item**  
2. **Understand what is required**  
3. **Ask for missing information**  
4. **Execute the task**  
5. **Document the results (report)**  
6. **Check off the item**  
7. **Work on the next item**

---

# **6. Rules for File Formats**

The agent uses the following formats:

| Purpose | Format |
|-------|--------|
| Prompts | `.md` |
| Reports | `.md` |
| TODO lists | `.md` |
| Concepts | `.md` |
| Documentation | `.md` |
| User manuals | `.md` |
| Scripts | `.sh`, `.ps1`, `.py`, `.bat` |
| Configuration | `.json`, `.yaml`, `.toml` |

Rules:

- Markdown is the default for all text-based content.  
- The agent produces **no binary files**, unless explicitly instructed.  
- The agent documents every generated file in a report.

---

# **7. Working Method of the AI Agent**

The agent works according to the following principles:

### **7.1 Step-by-Step Approach**
- The agent executes tasks in clear, traceable steps.
- Every step is documented.

### **7.2 Questions**
The agent asks questions when:

- information is missing  
- requirements are unclear  
- multiple interpretations are possible  

### **7.3 Consistency**
The agent:

- adheres to the project structure  
- uses consistent language  
- produces reproducible results  
- avoids unnecessary changes  

### **7.4 Safety**
The agent **never** overwrites:

- existing files  
- existing TODO items  

without explicit instruction.

---

# **8. Closing Rules**

- This document applies to **all AI agents**, regardless of model, version, or capabilities.  
- The agent reads this document **before every task**.  
- The agent strictly follows these rules.  
- Changes to this document may only be made by the user, not by the agent.

