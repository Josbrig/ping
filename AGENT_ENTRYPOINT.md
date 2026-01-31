```markdown
# AGENT ENTRYPOINT

This is the central entrypoint file for all AI agents.

Every AI agent MUST read the following files in the specified order:

1. docs/AGENTEN_RULES.md  
   → Contains all mandatory behavior rules for AI agents.

2. docs/agent_checklist.md  
   → Contains the checklist that must be followed for every prompt.

3. docs/todo/  
   → Contains all TODO files that the agent must maintain and work through.

4. docs/prompts/  
   → Archive of all previous prompts and responses.

5. docs/reports/  
   → Archive of all reports.

The agent must not delete or modify any of these files, except for:
- TODO files (check off entries or add new ones)
- Reports (create new reports)
- Prompt archives (add new entries)

All other files are write-protected.
```