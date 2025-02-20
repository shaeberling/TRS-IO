"use strict";

// Big thanks to http://www.trs-80.com/ for a majority of these!
function getMemoryRegions() {
    return [
      {
        "address": [0x3C00, 0x3FFF],
        "model_code": [1, 3],
        "description": "Video RAM"
      },
      {
        "address": [0x37E1],
        "model_code": [1],
        "description": "Disk drive select"
      },
      {
        "address": [0x37E4],
        "model_code": [1],
        "description": "Tape drive select: 0=#1 1=#2"
      },
      {
        "address": [0x37E8],
        "model_code": [1, 3],
        "description": "Printer Status: 63=On, 143=Off"
      },
      {
        "address": [0x37E9],
        "model_code": [1, 3],
        "description": "Printer output"
      },
      {
        "address": [0x37EC],
        "model_code": [1],
        "description": "Disk command/status"
      },
      {
        "address": [0x37ED],
        "model_code": [1],
        "description": "Disk track select"
      },
      {
        "address": [0x37EE],
        "model_code": [1],
        "description": "Disk sector select"
      },
      {
        "address": [0x37EF],
        "model_code": [1],
        "description": "Disk data"
      },
      {
        "address": [0x3800, 0x3840],
        "model_code": [1, 3],
        "description": "Keyboard matrix"
      },
      {
        "address": [0x3FCD, 0x3FCE],
        "model_code": [],
        "description": "End of BASIC Program Pointer (LSB)"
      },
      {
        "address": [0x4000],
        "model_code": [],
        "description": "RST 08 (Syntax Check)"
      },
      {
        "address": [0x4001, 0x4014],
        "model_code": [200],
        "description": "Jump vectors for RST 8 - RST 56"
      },
      {
        "address": [0x4003, 0x4005],
        "model_code": [1, 3],
        "description": "RST 10H (Get Next Character): Jump Vector."
      },
      {
        "address": [0x4006],
        "model_code": [],
        "description": "RST 18H (Cp HL)"
      },
      {
        "address": [0x4009],
        "model_code": [],
        "description": "RST 20H (Get Current Type) Jump Vector."
      },
      {
        "address": [0x400C, 0x400E],
        "model_code": [1, 3],
        "description": "RST 28H (Break Key Vector).  By default"
      },
      {
        "address": [0x400F],
        "model_code": [3],
        "description": "RST 30H Jump Vector."
      },
      {
        "address": [0x4012, 0x4015],
        "model_code": [1, 3],
        "description": "RST 38H Jump Vector."
      },
      {
        "address": [0x4015, 0x401C],
        "model_code": [1, 3],
        "description": "Beginning of the Keyboard Device Control Block (DCB)"
      },
      {
        "address": [0x4015],
        "model_code": [3],
        "description": "Tape RAM - Keyboard DCB: Type = 1 = Read Only"
      },
      {
        "address": [0x4016, 0x4017],
        "model_code": [1],
        "description": "Two byte keyboard driver vector"
      },
      {
        "address": [0x4016],
        "model_code": [3],
        "description": "Two byte keyboard driver vector."
      },
      {
        "address": [0x4018],
        "model_code": [1, 3],
        "description": "Keyboard DCB: Right Shift Toggle"
      },
      {
        "address": [0x4019],
        "model_code": [3],
        "description": "Caps Lock Toggle.  0=Off (i.e."
      },
      {
        "address": [0x401A],
        "model_code": [3],
        "description": "Cursor Blink Count."
      },
      {
        "address": [0x401B],
        "model_code": [3],
        "description": "Cursor Blink Status. (0=Off Nz=On)"
      },
      {
        "address": [0x401C],
        "model_code": [1, 3],
        "description": "Cursor Blink Switch.  Set to 0 for blink"
      },
      {
        "address": [0x401D, 0x4024],
        "model_code": [1, 3],
        "description": "Beggining of Video Display Control Block (DCB)."
      },
      {
        "address": [0x401D],
        "model_code": [3],
        "description": "Tape RAM - Video DCB: Type = 7 = Read/write"
      },
      {
        "address": [0x401E, 0x401F],
        "model_code": [],
        "description": "Two byte video driver vector"
      },
      {
        "address": [0x401E],
        "model_code": [3],
        "description": "Tape RAM - Video DCB: Driver Address (0473h)"
      },
      {
        "address": [0x4020, 0x4021],
        "model_code": [1, 3],
        "description": "Cursor Position On Screen. 2 Bytes in LSB/MSB Order"
      },
      {
        "address": [0x4022],
        "model_code": [],
        "description": "Cursor (0=on)"
      },
      {
        "address": [0x4022],
        "model_code": [3],
        "description": "Cursor On/off Flag (z = Off Nz = Character Undercursor)"
      },
      {
        "address": [0x4023],
        "model_code": [],
        "description": "Cursor Character (in ASCII). Defaults: 176."
      },
      {
        "address": [0x4023],
        "model_code": [3],
        "description": "Tape RAM - Video DCB: Cursor Character (default 0b0h)"
      },
      {
        "address": [0x4024],
        "model_code": [],
        "description": "FLAG : 0 = Space compression; Not 0 = special character."
      },
      {
        "address": [0x4024],
        "model_code": 3,
        "description": "Character Set To Use:  0=Regular"
      },
      {
        "address": [0x4024],
        "model_code": [3],
        "description": "Tape RAM - Video DCB: Tabs/special Characters Switch (z = Tabs)"
      }
    ];
    }

    const numToHex = (num) => ((num <= 0xF ? "0" : "") + num.toString(16)).toUpperCase();
    const CARRY_MASK = 0x1;
    const SUBTRACT_MASK = 0x2;
    const OVERFLOW_MASK = 0x4;
    const UNDOC3_MASK = 0x8;
    const HALF_CARRY_MASK = 0x10;
    const UNDOC5_MASK = 0x20;
    const ZERO_MASK = 0x40;
    const SIGN_MASK = 0x80;
    // Rendering parameters for the memory map.
    const BYTE_RENDER_GAP = 1;
    const BYTE_SIZE_PX = 8;
    const NUM_BYTES_X = 256; //132;
    const NUM_BYTES_Y = 128; // 132;
    // See web_debugger.h for definitions.
    const BP_TYPE_TEXT = ["Program Counter", "Memory Watch", "IO Watch"];
    const M3_TO_UTF = [
        "\u0020", "\u00a3", "\u007c", "\u00e9", "\u00dc", "\u00c5", "\u00ac", "\u00f6",
        "\u00d8", "\u00f9", "\u00f1", "\u0060", "\u0101", "\ue00d", "\u00c4", "\u00c3",
        "\u00d1", "\u00d6", "\u00d8", "\u00d5", "\u00df", "\u00fc", "\u00f5", "\u00e6",
        "\u00e4", "\u00e0", "\u0227", "\ue01b", "\u00c9", "\u00c6", "\u00c7", "\u02dc",
        "\u0020", "\u0021", "\u0022", "\u0023", "\u0024", "\u0025", "\u0026", "\u0027",
        "\u0028", "\u0029", "\u002a", "\u002b", "\u002c", "\u002d", "\u002e", "\u002f",
        "\u0030", "\u0031", "\u0032", "\u0033", "\u0034", "\u0035", "\u0036", "\u0037",
        "\u0038", "\u0039", "\u003a", "\u003b", "\u003c", "\u003d", "\u003e", "\u003f",
        "\u0040", "\u0041", "\u0042", "\u0043", "\u0044", "\u0045", "\u0046", "\u0047",
        "\u0048", "\u0049", "\u004a", "\u004b", "\u004c", "\u004d", "\u004e", "\u004f",
        "\u0050", "\u0051", "\u0052", "\u0053", "\u0054", "\u0055", "\u0056", "\u0057",
        "\u0058", "\u0059", "\u005a", "\u005b", "\u005c", "\u005d", "\u005e", "\u005f",
        "\u0060", "\u0061", "\u0062", "\u0063", "\u0064", "\u0065", "\u0066", "\u0067",
        "\u0068", "\u0069", "\u006a", "\u006b", "\u006c", "\u006d", "\u006e", "\u006f",
        "\u0070", "\u0071", "\u0072", "\u0073", "\u0074", "\u0075", "\u0076", "\u0077",
        "\u0078", "\u0079", "\u007a", "\u007b", "\u007c", "\u007d", "\u007e", "\u00b1",
        "\ue080", "\ue081", "\ue082", "\ue083", "\ue084", "\ue085", "\ue086", "\ue087",
        "\ue088", "\ue089", "\ue08a", "\ue08b", "\ue08c", "\ue08d", "\ue08e", "\ue08f",
        "\ue090", "\ue091", "\ue092", "\ue093", "\ue094", "\ue095", "\ue096", "\ue097",
        "\ue098", "\ue099", "\ue09a", "\ue09b", "\ue09c", "\ue09d", "\ue09e", "\ue09f",
        "\ue0a0", "\ue0a1", "\ue0a2", "\ue0a3", "\ue0a4", "\ue0a5", "\ue0a6", "\ue0a7",
        "\ue0a8", "\ue0a9", "\ue0aa", "\ue0ab", "\ue0ac", "\ue0ad", "\ue0ae", "\ue0af",
        "\ue0b0", "\ue0b1", "\ue0b2", "\ue0b3", "\ue0b4", "\ue0b5", "\ue0b6", "\ue0b7",
        "\ue0b8", "\ue0b9", "\ue0ba", "\ue0bb", "\ue0bc", "\ue0bd", "\ue0be", "\ue0bf",
        "\u2660", "\u2665", "\u2666", "\u2663", "\u263a", "\u2639", "\u2264", "\u2265",
        "\u03b1", "\u03b2", "\u03b3", "\u03b4", "\u03b5", "\u03b6", "\u03b7", "\u03b8",
        "\u03b9", "\u03ba", "\u03bc", "\u03bd", "\u03be", "\u03bf", "\u03c0", "\u03c1",
        "\u03c2", "\u03c3", "\u03c4", "\u03c5", "\u03c6", "\u03c7", "\u03c8", "\u03c9",
        "\u2126", "\u221a", "\u00f7", "\u2211", "\u2248", "\u2206", "\u2307", "\u2260",
        "\u2301", "\ue0e9", "\u237e", "\u221e", "\u2713", "\u00a7", "\u2318", "\u00a9",
        "\u00a4", "\u00b6", "\u00a2", "\u00ae", "\ue0f4", "\ue0f5", "\ue0f6", "\u211e",
        "\u2105", "\u2642", "\u2640", "\ue0fb", "\ue0fc", "\ue0fd", "\ue0fe", "\u2302"
    ];
    var MouseAction;
    (function (MouseAction) {
        MouseAction[MouseAction["MOVE"] = 1] = "MOVE";
        MouseAction[MouseAction["UP"] = 2] = "UP";
        MouseAction[MouseAction["DOWN"] = 3] = "DOWN";
    })(MouseAction || (MouseAction = {}));
    class TrsXray {
        constructor() {
            this.socket = null;
            this.lastBreakpoints = Array(0);
            this.canvas = document.getElementById("memory-container");
            this.ctx = this.canvas.getContext("2d");
            this.programCounter = 0;
            this.prevProgramCounter = 0;
            this.stackPointer = 0;
            this.memRegions = getMemoryRegions();
            this.memInfo = new Map();
            this.memoryData = new Uint8Array(0xFFFF);
            this.memoryChanged = new Uint8Array(0xFFFF);
            this.lastByteColor = new Array(0xFFFF);
            this.selectedMemoryRegion = -1;
            this.hoveredByte = -1;
            this.selectedByte = -1;
            this.isMouseOnScreen = false;
            this.enableDataViz = false;
            this.enableLineViz = false;
            this.enableFullMemoryUpdate = true;
            this.memoryUpdateStartAddress = 0;
            this.memRegions.map((region, idx) => {
                for (let i = region.address[0]; i <= region.address[region.address.length - 1]; ++i) {
                    this.memInfo.set(i, idx);
                }
            });
            this.initCanvas();
        }
        initCanvas() {
            const totalByteSize = BYTE_SIZE_PX + BYTE_RENDER_GAP;
            this.canvas.width = NUM_BYTES_X * totalByteSize;
            this.canvas.height = NUM_BYTES_Y * totalByteSize;
            this.canvas.style.width = this.canvas.width + "px";
            this.canvas.style.height = this.canvas.height + "px";
        }
        onMessageFromEmulator(json) {
            if (json.context)
                this.onContextUpdate(json.context);
            if (json.breakpoints)
                this.onBreakpointUpdate(json.breakpoints);
            if (json.registers)
                this.onRegisterUpdate(json.registers);
        }
        onControl(action) {
            if (this.socket != undefined)
                this.socket.send("action/" + action);
        }
        requestMemoryUpdate() {
            if (this.enableFullMemoryUpdate) {
                this.memoryUpdateStartAddress = 0;
                this.onControl("get_memory/0/65536");
            }
            else {
                this.memoryUpdateStartAddress = 0x3C00;
                this.onControl(`get_memory/${0x3C00}/${0x3FFF - 0x3C00}`);
            }
        }
        /** Key pressed that is to be send to SUT. */
        onKeyPressForSut(direction, evt) {
            evt.preventDefault();
            if (evt.repeat)
                return;
            let shift = evt.shiftKey ? "1" : "0";
            let dir = direction == "down" ? "1" : "0";
            this.onControl(`key_event/${dir}/${shift}/${evt.key}`);
        }
        onLoad() {
            $('input:text').on("keydown", (evt) => { evt.stopPropagation(); });
            document.addEventListener("keydown", (evt) => {
                if (this.isMouseOnScreen)
                    this.onKeyPressForSut("down", evt);
                else {
                    switch (evt.key) {
                        case 'j':
                            this.onControl("step");
                            this.requestMemoryUpdate();
                            break;
                        case 't':
                            this.debug_insertTestData();
                            break;
                        case 'd':
                            this.enableDataViz = !this.enableDataViz;
                            break;
                        case 'e':
                            this.enableLineViz = !this.enableLineViz;
                            break;
                        case 'm':
                            this.enableFullMemoryUpdate = !this.enableFullMemoryUpdate;
                            break;
                        case 'r':
                            this.onControl("get_memory/force_update");
                            break;
                        default:
                            console.log(`Unhandled key event: ${evt.key}`);
                    }
                }
            });
            document.addEventListener("keyup", (evt) => {
                if (this.isMouseOnScreen)
                    this.onKeyPressForSut("up", evt);
            });
            $("#step-btn").on("click", () => {
                this.onControl("step");
                this.requestMemoryUpdate();
            });
            $("#step-over-btn").on("click", () => { this.onControl("step-over"); });
            $("#play-btn").on("click", () => { this.onControl("continue"); });
            $("#stop-btn").on("click", () => { this.onControl("stop"); });
            $("#reset-btn").on("click", (ev) => {
                // this.onControl(ev.shiftKey ? "hard_reset" : "soft_reset")
                this.onControl("get_memory/force_update");
            });
            const addBreakpointHandler = (ev) => {
                const type = $(ev.currentTarget).attr("data");
                console.log(`Click data: ${ev.currentTarget} -> ${type}`);
                const addr1 = $("#selected-addr-1").val();
                const addr2 = $("#selected-addr-2").val();
                console.log(`Add breakpoint for: ${addr1}/${addr2}`);
                if (!addr1 || addr1.length != 2 || !addr2 || addr2.length != 2) {
                    alert("Invalid address");
                    return;
                }
                const addr = parseInt(`${addr1}${addr2}`, 16);
                this.onControl(`add_breakpoint/${type}/${addr}`);
            };
            $("#add-breakpoint-pc").on("click", addBreakpointHandler);
            $("#add-breakpoint-memory").on("click", addBreakpointHandler);
            $("#memory-container").on("mousemove", (evt) => {
                this.onMouseActionOnCanvas(evt.offsetX, evt.offsetY, MouseAction.MOVE);
            });
            $("#memory-container").on("mouseup", (evt) => {
                this.onMouseActionOnCanvas(evt.offsetX, evt.offsetY, MouseAction.UP);
            });
            $("#screen").on("mouseenter", () => { this.isMouseOnScreen = true; })
                .on("mouseleave", () => { this.isMouseOnScreen = false; });
            this.createMemoryRegions();
            if (!isDebugMode())
                this.keepConnectionAliveLoop();
        }
        keepConnectionAliveLoop() {
            if (!this.socket || this.socket.readyState != WebSocket.OPEN) {
                $("h1").addClass("errorTitle");
                if (!this.socket ||
                    this.socket.readyState == WebSocket.CLOSED ||
                    this.socket.readyState == WebSocket.CLOSING) {
                    this.createNewSocket();
                }
                setTimeout(() => this.keepConnectionAliveLoop(), 200);
            }
            else {
                $("h1").removeClass("errorTitle");
                setTimeout(() => this.keepConnectionAliveLoop(), 500);
            }
        }
        createNewSocket() {
            console.log("Creating new Websocket");
            this.socket = new WebSocket("ws://" + location.host + "/channel");
            this.socket.onerror = (evt) => {
                console.log("Unable to connect to websocket.");
                $("h1").addClass("errorTitle");
            };
            this.socket.onopen = () => {
                console.log("Socked opened.");
                this.onControl("refresh");
            };
            this.socket.onmessage = (evt) => {
                if (evt.data instanceof Blob) {
                    evt.data.arrayBuffer().then((data) => {
                        this.onMemoryUpdate(new Uint8Array(data));
                        // Unnecessary as we also request a register update that will queue
                        // a full memory region draw event.
                        // this.renderMemoryRegions();
                        this.renderDisplay();
                    });
                }
                else {
                    var json = JSON.parse(evt.data);
                    this.onMessageFromEmulator(json);
                    this.renderMemoryRegions();
                }
            };
        }
        onContextUpdate(ctx) {
            $("#sut-name").text(ctx.system_name);
            $("#sut-model-no").text(ctx.model);
        }
        onRegisterUpdate(registers) {
            /* Main registers. */
            let a = (registers.af & 0xFF00) >> 8;
            let f = (registers.af & 0x00FF);
            let b = (registers.bc & 0xFF00) >> 8;
            let c = (registers.bc & 0x00FF);
            let d = (registers.de & 0xFF00) >> 8;
            let e = (registers.de & 0x00FF);
            let h = (registers.hl & 0xFF00) >> 8;
            let l = (registers.hl & 0x00FF);
            /* Alternate registers. */
            let ap = (registers.af_prime & 0xFF00) >> 8;
            let fp = (registers.af_prime & 0x00FF);
            let bp = (registers.bc_prime & 0xFF00) >> 8;
            let cp = (registers.bc_prime & 0x00FF);
            let dp = (registers.de_prime & 0xFF00) >> 8;
            let ep = (registers.de_prime & 0x00FF);
            let hp = (registers.hl_prime & 0xFF00) >> 8;
            let lp = (registers.hl_prime & 0x00FF);
            /* Index registers. */
            let ix1 = (registers.ix & 0xFF00) >> 8;
            let ix2 = (registers.ix & 0x00FF);
            let iy1 = (registers.iy & 0xFF00) >> 8;
            let iy2 = (registers.iy & 0x00FF);
            let sp1 = (registers.sp & 0xFF00) >> 8;
            let sp2 = (registers.sp & 0x00FF);
            /* Program counter. */
            let pc1 = (registers.pc & 0xFF00) >> 8;
            let pc2 = (registers.pc & 0x00FF);
            /** Extract flag values from 'f' register. */
            let flag_sign = f & SIGN_MASK;
            let flag_zero = f & ZERO_MASK;
            let flag_undoc5 = f & UNDOC5_MASK;
            let flag_half_carry = f & HALF_CARRY_MASK;
            let flag_undoc3 = f & UNDOC3_MASK;
            let flag_overflow = f & OVERFLOW_MASK;
            let flag_subtract = f & SUBTRACT_MASK;
            let flag_carry = f & CARRY_MASK;
            $("#reg-af-1").val(numToHex(a));
            $("#reg-af-2").val(numToHex(f));
            $("#reg-bc-1").val(numToHex(b));
            $("#reg-bc-2").val(numToHex(c));
            $("#reg-de-1").val(numToHex(d));
            $("#reg-de-2").val(numToHex(e));
            $("#reg-hl-1").val(numToHex(h));
            $("#reg-hl-2").val(numToHex(l));
            $("#reg-afp-1").val(numToHex(ap));
            $("#reg-afp-2").val(numToHex(fp));
            $("#reg-bcp-1").val(numToHex(bp));
            $("#reg-bcp-2").val(numToHex(cp));
            $("#reg-dep-1").val(numToHex(dp));
            $("#reg-dep-2").val(numToHex(ep));
            $("#reg-hlp-1").val(numToHex(hp));
            $("#reg-hlp-2").val(numToHex(lp));
            $("#reg-ix-1").val(numToHex(ix1));
            $("#reg-ix-2").val(numToHex(ix2));
            $("#reg-iy-1").val(numToHex(iy1));
            $("#reg-iy-2").val(numToHex(iy2));
            $("#reg-sp-1").val(numToHex(sp1));
            $("#reg-sp-2").val(numToHex(sp2));
            $("#reg-pc-1").val(numToHex(pc1));
            $("#reg-pc-2").val(numToHex(pc2));
            /* Fun with Flags */
            let setFlag = (id, value) => {
                if (value) {
                    $(id).addClass("flag-enabled");
                }
                else {
                    $(id).removeClass("flag-enabled");
                }
            };
            setFlag("#flag-s", flag_sign);
            setFlag("#flag-z", flag_zero);
            setFlag("#flag-u5", flag_undoc5);
            setFlag("#flag-h", flag_half_carry);
            setFlag("#flag-u3", flag_undoc3);
            setFlag("#flag-pv", flag_overflow);
            setFlag("#flag-n", flag_subtract);
            setFlag("#flag-c", flag_carry);
            this.prevProgramCounter = this.programCounter;
            this.programCounter = registers.pc;
            this.stackPointer = registers.sp;
        }
        onBreakpointUpdate(breakpoints) {
            if (JSON.stringify(this.lastBreakpoints) == JSON.stringify(breakpoints))
                return;
            this.lastBreakpoints = breakpoints;
            console.log(JSON.stringify(this.lastBreakpoints));
            console.log(JSON.stringify(breakpoints));
            $("#breakpoints").empty();
            for (var bp of breakpoints) {
                let r1 = (bp.address & 0xFF00) >> 8;
                let r2 = (bp.address & 0x00FF);
                $(`<div class="register">${numToHex(r1)}</div>`).appendTo("#breakpoints");
                $(`<div class="register">${numToHex(r2)}</div>`).appendTo("#breakpoints");
                $(`<div class="breakpoint-type">${BP_TYPE_TEXT[bp.type]}</div>`).appendTo("#breakpoints");
                $(`<div class="remove-breakpoint" data="${bp.id}"></div>`)
                    .on("click", (evt) => {
                    const id = $(evt.currentTarget).attr("data");
                    this.onControl(`remove_breakpoint/${id}`);
                })
                    .appendTo("#breakpoints");
            }
        }
        onMemoryUpdate(memory) {
            // The first two bytes are the start offset address.
            let startAddr = (memory[0] << 8) + memory[1];
            console.log(`Starting Addr: ${startAddr}`);
            this.memoryChanged = new Uint8Array(0xFFFF);
            for (let i = 2; i < memory.length; ++i) {
                let addr = startAddr + i - 2;
                if (this.memoryData[addr] != memory[i]) {
                    this.memoryChanged[addr] = 1;
                    this.memoryData[addr] = memory[i];
                }
            }
            this.onSelectionUpdate();
        }
        onSelectionUpdate() {
            let hi = (this.selectedByte & 0xFF00) >> 8;
            let lo = (this.selectedByte & 0x00FF);
            $("#selected-addr-1").val(numToHex(hi));
            $("#selected-addr-2").val(numToHex(lo));
            if (this.memoryData && this.selectedByte >= 0) {
                $("#selected-val").val(numToHex(this.memoryData[this.selectedByte]));
            }
        }
        createMemoryRegions() {
            const regions = getMemoryRegions();
            const container = $("#memory-regions");
            regions.map((region, idx) => {
                $("<div></div>")
                    .addClass("map-region-list-entry")
                    .text(region.description)
                    .on("mouseover", () => { this.onMemoryRegionSelect(idx); })
                    .appendTo(container);
            });
            this.renderMemoryRegions();
        }
        onMemoryRegionSelect(region) {
            this.selectedMemoryRegion = region;
            this.renderMemoryRegions();
        }
        getColorForByte(addr) {
            // Set base color for addresses we have data for
            let color = !!this.memInfo.get(addr) ? "#666" : "#444";
            // Any byte with value zero get blacked out.
            if (this.memoryData && this.memoryData[addr] == 0)
                color = "#000";
            if (this.enableDataViz && !!this.memoryData) {
                let hexValue = numToHex(this.memoryData[addr] >> 1);
                if (hexValue.length == 1)
                    hexValue = `0${hexValue}`;
                color = `#${hexValue}${hexValue}${hexValue}`;
            }
            // Mark selected range bytes.
            if (this.selectedMemoryRegion >= 0) {
                let addressRange = this.memRegions[this.selectedMemoryRegion].address;
                const highlightStart = addressRange[0];
                const highlightEnd = addressRange[addressRange.length - 1];
                color = (addr >= highlightStart && addr <= highlightEnd) ? "#F00" : color;
            }
            // Highlight bytes that have changed with the most recent update.
            if (this.memoryChanged.length > addr && this.memoryChanged[addr] == 1) {
                color = "#FFA500";
            }
            // Mark special program and stack pointers.
            if (addr == this.programCounter)
                color = "#0F0";
            if (addr == this.stackPointer)
                color = "#FF0";
            return color;
        }
        renderByte(x, y, removeHighlight = false) {
            const addr = (y * NUM_BYTES_X) + x;
            const totalByteSize = BYTE_SIZE_PX + BYTE_RENDER_GAP;
            const changeHighlight = addr == this.selectedByte ||
                addr == this.hoveredByte ||
                removeHighlight;
            if (changeHighlight) {
                this.ctx.fillStyle = addr == this.hoveredByte ? "#88A" :
                    (addr == this.selectedByte ? "#FFF" : "#000");
                this.ctx.fillRect(x * totalByteSize - 1, y * totalByteSize - 1, BYTE_SIZE_PX + 2, BYTE_SIZE_PX + 2);
            }
            // Optimize rendering by only updating changed bytes.
            // ~10x speed improvement.
            const color = this.getColorForByte(addr);
            if (color != this.lastByteColor[addr] || changeHighlight) {
                this.ctx.fillStyle = color;
                this.ctx.fillRect(x * totalByteSize, y * totalByteSize, BYTE_SIZE_PX, BYTE_SIZE_PX);
                this.lastByteColor[addr] = color;
            }
        }
        renderEffects() {
            const totalByteSize = BYTE_SIZE_PX + BYTE_RENDER_GAP;
            const addrPc0Y = Math.floor(this.prevProgramCounter / NUM_BYTES_X);
            const addrPc0X = this.prevProgramCounter % NUM_BYTES_X;
            const addrPc1Y = Math.floor(this.programCounter / NUM_BYTES_X);
            const addrPc1X = this.programCounter % NUM_BYTES_X;
            const pc0Y = addrPc0Y * totalByteSize + BYTE_SIZE_PX / 2;
            const pc0X = addrPc0X * totalByteSize + BYTE_SIZE_PX / 2;
            const pc1Y = addrPc1Y * totalByteSize + BYTE_SIZE_PX / 2;
            const pc1X = addrPc1X * totalByteSize + BYTE_SIZE_PX / 2;
            console.log(`Line: ${pc0X},${pc0Y} -> ${pc1X},${pc1Y}`);
            this.ctx.beginPath();
            this.ctx.strokeStyle = "yellow";
            this.ctx.moveTo(pc0X, pc0Y);
            this.ctx.lineTo(pc1X, pc1Y);
            this.ctx.stroke();
        }
        renderMemoryRegions() {
            // console.time("renderMemoryRegions");
            for (let y = 0; y < NUM_BYTES_Y; ++y) {
                for (let x = 0; x < NUM_BYTES_X; ++x) {
                    this.renderByte(x, y);
                }
            }
            if (this.enableLineViz)
                this.renderEffects();
            // console.timeEnd("renderMemoryRegions");
        }
        onMouseActionOnCanvas(x, y, a) {
            let setter = a == MouseAction.MOVE ?
                (newAddr) => { this.hoveredByte = newAddr; } :
                (newAddr) => { this.selectedByte = newAddr; };
            let getter = a == MouseAction.MOVE ?
                () => { return this.hoveredByte; } :
                () => { return this.selectedByte; };
            const totalByteSize = BYTE_SIZE_PX + BYTE_RENDER_GAP;
            const byteX = Math.floor(x / totalByteSize);
            const byteY = Math.floor(y / totalByteSize);
            const prevByte = getter();
            setter(byteY * NUM_BYTES_X + byteX);
            const prevByteY = Math.floor(prevByte / NUM_BYTES_X);
            const prevByteX = prevByte - (prevByteY * NUM_BYTES_X);
            this.renderByte(prevByteX, prevByteY, true);
            this.renderByte(byteX, byteY);
            if (a == MouseAction.UP) {
                this.onSelectionUpdate();
            }
        }
        renderDisplay() {
            // console.time("renderDisplay");
            if (!this.memoryData)
                return;
            // FIXME: Need to determine when we have 64x16 or 80x24
            const width = 64;
            const height = 16;
            const VIDEO_RAM_OFFSET = 0x3C00;
            let screenStr = "";
            for (let y = 0; y < height; ++y) {
                for (let x = 0; x < width; ++x) {
                    screenStr += M3_TO_UTF[this.memoryData[y * width + x + VIDEO_RAM_OFFSET]];
                }
                screenStr += "\n";
            }
            $("#screen").text(screenStr);
            // console.timeEnd("renderDisplay");
        }
        debug_insertTestData() {
            console.log("Inserting test data for debugging...");
            const data = { "context": { "system_name": "sdlTRS", "model": 3 }, "breakpoints": [{ "id": 0, "address": 4656, "type": 0 }, { "id": 1, "address": 6163, "type": 0 }, { "id": 2, "address": 9545, "type": 0 }], "registers": { "pc": 2, "sp": 65535, "af": 68, "bc": 0, "de": 0, "hl": 0, "af_prime": 0, "bc_prime": 0, "de_prime": 0, "hl_prime": 0, "ix": 0, "iy": 0, "i": 0, "r_1": 0, "r_2": 2, "z80_t_state_counter": 8, "z80_clockspeed": 2.0299999713897705, "z80_iff1": 0, "z80_iff2": 0, "z80_interrupt_mode": 0 } };
            this.onMessageFromEmulator(data);
        }
    }
    //# sourceMappingURL=web_debugger.js.map