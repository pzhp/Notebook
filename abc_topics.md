# Technical English Expressions — By Topic

All 77 expressions from the raw notes, grouped by semantic theme. Within each theme, expressions are ordered by frequency (very high → high → medium).

---

## Theme 1: Giving Feedback & Review

### Very high frequency
- **trade-off** — "This design improves throughput, but the trade-off is higher CPU overhead."
- **no concrete data** — "We do not have concrete data yet, so this is still a hypothesis."
- **actionable plan** — "The report needs an actionable plan with owners and deadlines."
- **one-size-fits-all** — "This is not a one-size-fits-all solution; we need per-workload tuning."
- **feasible** — "Sub-second batch latency is feasible with the current architecture."

### High frequency
- **substantial change** — "This refactor leads to a substantial change in execution flow."
- **incur technical debt** — "This shortcut helps us ship early, but it incurs technical debt."
- **deep dive / delve into** — "We did a deep dive into the root cause of the latency regression."
- **outside the scope of** — "A detailed description is outside the scope of this document."
- **postmortem** — "A prompt postmortem tends to be more accurate because information is fresh."

### Medium frequency
- **suboptimal** — "This is a suboptimal choice for bursty workloads."
- **runtime penalty** — "This plugin mechanism introduces a runtime penalty."
- **compelling** — "The benchmark results make a compelling case for migration."
- **boil down to** — "The decision boils down to these key questions."
- **substantial effort** — "The change is feasible, but it involves substantial effort."

---

## Theme 2: Expressing Uncertainty & Assumptions

### Very high frequency
- **likely / unlikely** — "The issue is unlikely, but we still need a mitigation plan."
- **no concrete data** — "We do not have concrete data yet, so this is still a hypothesis."
- **assumption** — "In the absence of hard evidence, we should treat this as a working assumption."

### High frequency
- **infer / deduce** — "Followers infer leader failure from missing heartbeats."
- **implementation-dependent** — "The final behavior is implementation-dependent."
- **proven causal relationship** — "There is a proven causal relationship between heap size and GC pause duration."
- **in the absence of (hard evidence)** — "In the absence of hard evidence, the theory flourished unchallenged."
- **deviate from** — "The actual situation may deviate from the original assumption."

### Medium frequency
- **counterintuitive** — "The result may sound counterintuitive, but it is backed by data."

---

## Theme 3: Performance & Scaling

### Very high frequency
- **scale linearly** — "Throughput scales linearly with the number of workers."
- **backpressure** — "The queue builds backpressure under bursty workloads."
- **lock contention** — "Lock contention is the main bottleneck in this path."
- **room for optimization** — "There is still room for optimization in memory allocation."
- **upper bound / lower bound** — "We need a fixed upper bound on retry delay."
- **keep up with** — "The current architecture cannot keep up with demand."
- **latency spikes** — "We can reduce latency spikes without compromising performance."
- **negligible** — "Although the performance degradation is negligible, there is no gain either."
- **tightly coupled** — "These services are tightly coupled in a single application."
- **without compromising performance** — "We can reduce latency spikes without compromising performance."
- **edge case** — "We ran into edge cases that caused conflicts during migration."

### High frequency
- **performance degradation** — "Excessive growth causes performance degradation over time."
- **deterministic** — "The behavior is more deterministic with explicit resource isolation."
- **overhead (outweigh benefits)** — "The overhead might outweigh the benefits for this workload."
- **proportionally / in proportion to** — "Throughput reduces proportionally to mean task duration."

### Medium frequency
- **eventually exhaust** — "Without cleanup, the process may eventually exhaust resources."
- **order of magnitude** — "Switching to batch inserts gave us an order-of-magnitude improvement."
- **contend for (resource)** — "When too many pods contend for CPU, scheduling latency increases."
- **CPU starvation** — "Under heavy load, background threads experience CPU starvation."
- **soft limit / hard limit** — "The hard limit acts as a ceiling for the soft limit."
- **drastically (reduce)** — "This fix drastically reduces occurrence of this issue."

---

## Theme 4: Reliability & Recovery

### Very high frequency
- **idempotent** — "The handler is idempotent, so retries are safe."
- **exponential backoff** — "We retry failed requests with exponential backoff."
- **stale data** — "Clients may read stale data during leader transitions."

### High frequency
- **resilient to failures** — "The Deployment makes the scheduler resilient to failures."
- **failure visibility** — "Failure visibility must be deliberate, not accidental."
- **immune to** — "Your code will be immune to SQL injection by design."
- **race condition** — "Using close() and dup() separately would be subject to race conditions."
- **preemptively** — "A proper CPU power cap is set preemptively when the temperature exceeds the threshold."
- **eventually consistent** — "Reads are eventually consistent during recovery."
- **partition tolerance** — "The system prioritizes partition tolerance over strict consistency."

### Medium frequency
- **as a last resort** — "Use forced termination only as a last resort."
- **defeat the purpose** — "Skipping validation defeats the goal of the redesign entirely."
- **panacea** — "File locks are not a panacea; they have known issues."

---

## Theme 5: Architecture & Boundaries

### Very high frequency
- **subject to (change)** — "These field values are subject to change across versions."

### High frequency
- **precedence** — "The pod-level value takes precedence over the default setting."
- **equivalent to** — "This pipeline is logically equivalent to the visitor model."
- **conform to standards** — "The payload must conform to the JSON spec."
- **by default** — "The pod is scheduled by default unless a scheduler name is supplied."
- **verify in event logs** — "We can verify the result in event logs."

### Medium frequency
- **one-way operation** — "Namespace visibility is effectively a one-way operation."
- **take advantage of** — "We can take advantage of the aggregation layer to extend the API."
- **highly configurable and extensible** — "Kubernetes is highly configurable and extensible."
- **canonical** — "This is the canonical way to describe the relationship."
- **coherent** — "We need a coherent system architecture before scaling further."
- **complement each other** — "These two approaches complement each other well in production."
- **arbitrary** — "CycleState provides a mechanism for plugins to store and retrieve arbitrary data."
- **delicate balance** — "Namespace design is a delicate balance between security and usability."
- **supersede / step down** — "The leader may have been superseded by a newer leader."

---

## Theme 6: Describing Quality & Risk

### Very high frequency
- **brittle** — "String matching on error text is brittle and hard to maintain."
- **error-prone** — "The implementation is redundant and prone to errors."

### High frequency
- **swallow errors** — "This wrapper swallows errors and reduces observability."
- **deliberately / intentionally** — "The process deliberately ignores SIGTERM to prevent premature termination."
- **inadvertently** — "This change could inadvertently grant more permissions than intended."
