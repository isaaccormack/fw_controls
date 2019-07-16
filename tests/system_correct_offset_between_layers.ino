/* Test Case: Start of filament wind layer is correctly offset between layers

  Setup: Ensure that the arduino.json file in the .vscode folder is loading the main.ino file by
         setting: "sketch": "src/main.ino".
  
  Precondition: The system_correct_passes_for_layers, system_accurate_delay_distance_between_passes and
                system_consistency_of_passes_during_wind tests have been run and passed. This test cannot
                yield meaningful results if the aforementioned conditions are not true.

  Units Under Test:
    Physical: All - As in, error in any physical configuration could result in failure of test case.

    Software:
      In addition to the below member functions and variables, the implementation of the control logic
      in the main winding loop is also tested.
      - config::passes_per_layer
      - Mandrel.set_step_count_at_start_of_pass_even();
      - Mandrel.set_step_count_at_start_of_pass_odd();

  Background: The machine is intended to begin each successive winding layer by incremementing the step count
              by half the effective filament width such that the filament strands on the current layer lie in the 
              middle between the previous two filament strands. To mitigate repeated overlap of filament that would
              occur if the start of each layer began at either of the two positions, the wind layer always starts
              half the filament width after the last start position, such that there would need to be total_passes
              number of layers for the same start of layer position to occur twice.

  Test Parameters:
    - config::total_layers should be tested for the values [2, 3, 4, 5]
    - config::filament_offset should be tested with 1in and possibly 1/2in if desired.

  Procedure:
    1. Set the config::filament_percent_overlap = 0. 
    2. Attach a marker to the end of the filament application head (likely via rubber bands).
    3. Tape paper around the mandrel covering the an area large enough to see the offsets between start of wind position.
    4. Run main.ino
    5. Draw the first pass of each wind layer on the paper via marker attached to carriage. Be sure to diligently watch
       the motion of the machine as to predict the start of the next layer.

  Expected Results:
    - The perpendicular distance the angled lines marking the start of each layer should be half the specified filament
      width and each layer should always start half the filament width after the last one.  

  LAST EXECUTED RESULTS:
    - (July 15, 2019) The perpendicular distance between the start of each layer correct with +- 1/32" and each layer
      started the correct distance after the last one. Note that data collection must be done dilligently by anticipating
      the start of each wind layer via visual observation. Doing extraneous markings on the mandrel during a pass,
      especially near the area of interest in this test will make data harder to interpret and should be avoided. This may
      happen as the last pass of a wind will trace nearly over the previous start positions, which if recording produces
      ambiguity in results.
 */

// void setup() {}

// void loop() {}
